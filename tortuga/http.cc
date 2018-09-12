#include "tortuga/http.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "folly/String.h"
#include "folly/fibers/Baton.h"
#include "folly/io/async/EventBase.h"
#include "folly/io/async/SSLContext.h"
#include "folly/io/async/SSLOptions.h"
#include "proxygen/lib/http/HTTPConnector.h"
#include "proxygen/lib/http/HTTPMessage.h"
#include "proxygen/lib/http/session/HTTPTransaction.h"
#include "proxygen/lib/http/session/HTTPUpstreamSession.h"
#include "proxygen/lib/utils/URL.h"

namespace tortuga {
namespace {
class HttpHandler : public proxygen::HTTPConnector::Callback,
                    public proxygen::HTTPTransactionHandler {

 public:
  HttpHandler(folly::fibers::Baton* baton,
              folly::EventBase* evb,
              proxygen::HTTPMethod http_method,
              const proxygen::URL& url,
              const proxygen::HTTPHeaders& headers,
              const std::string* body);

  virtual ~HttpHandler() override = default;

  // initial SSL related structures
  void InitializeSsl(const std::string& next_protos);

  void sslHandshakeFollowup(proxygen::HTTPUpstreamSession* session) noexcept;

  // HTTPConnector methods
  void connectSuccess(proxygen::HTTPUpstreamSession* session) override;
  void connectError(const folly::AsyncSocketException& ex) override;

  // HTTPTransactionHandler methods
  void setTransaction(proxygen::HTTPTransaction* txn) noexcept override;
  void detachTransaction() noexcept override;
  void onHeadersComplete(std::unique_ptr<proxygen::HTTPMessage> msg) noexcept override;
  void onBody(std::unique_ptr<folly::IOBuf> chain) noexcept override;
  void onTrailers(std::unique_ptr<proxygen::HTTPHeaders> trailers) noexcept override;
  void onEOM() noexcept override;
  void onUpgrade(proxygen::UpgradeProtocol protocol) noexcept override;
  void onError(const proxygen::HTTPException& error) noexcept override;
  void onEgressPaused() noexcept override;
  void onEgressResumed() noexcept override;

  void SendRequest(proxygen::HTTPTransaction* txn);

  const std::string& get_server_name() const;
  folly::SSLContextPtr get_ssl_context() { return ssl_context_; }

  HttpResponse MakeResponse();

 protected:
  folly::fibers::Baton* baton_{ nullptr };
  proxygen::HTTPTransaction* txn_{nullptr};
  folly::EventBase* evb_{nullptr};
  proxygen::HTTPMethod http_method_;
  proxygen::URL url_;
  proxygen::HTTPMessage request_;
  folly::SSLContextPtr ssl_context_;
  const int32_t recv_window_{ 65536 };

  std::unique_ptr<proxygen::HTTPMessage> response_;
  std::unique_ptr<proxygen::HTTPException> error_;

 private:
  // body to send, not owned, may be null...
  const std::string* body_{ nullptr };
  std::unique_ptr<std::ostringstream> resp_body_;
};

HttpHandler::HttpHandler(folly::fibers::Baton* baton,
                         folly::EventBase* evb,
                         proxygen::HTTPMethod http_method,
                         const proxygen::URL& url,
                         const proxygen::HTTPHeaders& headers,
                         const std::string* body)
    : baton_(baton),
      evb_(evb),
      http_method_(http_method),
      url_(url),
      body_(body) {
  headers.forEach([this] (const std::string& header, const std::string& val) {
    request_.getHeaders().add(header, val);
  });
}

void HttpHandler::InitializeSsl(const std::string& next_protos) {
  ssl_context_ = std::make_shared<folly::SSLContext>();
  ssl_context_->setOptions(SSL_OP_NO_COMPRESSION);
  ssl_context_->setCipherList(folly::ssl::SSLCommonOptions::kCipherList);
  
  std::list<std::string> next_proto_list;
  folly::splitTo<std::string>(',', next_protos, std::inserter(next_proto_list,
                                                              next_proto_list.begin()));
  ssl_context_->setAdvertisedNextProtocols(next_proto_list);
}

void HttpHandler::sslHandshakeFollowup(proxygen::HTTPUpstreamSession* session) noexcept {
  folly::AsyncSSLSocket* sslSocket = dynamic_cast<folly::AsyncSSLSocket*>(session->getTransport());

  const unsigned char* next_proto = nullptr;
  uint32_t next_proto_length = 0;
  sslSocket->getSelectedNextProtocol(&next_proto, &next_proto_length);
  if (next_proto != nullptr) {
    VLOG(1) << "Client selected next protocol " << std::string((const char*) next_proto, next_proto_length);
  } else {
    VLOG(1) << "Client did not select a next protocol";
  }

  // Note: This ssl session can be used by defining a member and setting
  // something like sslSession_ = sslSocket->getSSLSession() and then
  // passing it to the connector::connectSSL() method
}

void HttpHandler::connectSuccess(proxygen::HTTPUpstreamSession* session) {
  if (url_.isSecure()) {
    sslHandshakeFollowup(session);
  }

  session->setFlowControl(recv_window_, recv_window_, recv_window_);
  SendRequest(session->newTransaction(this));
  session->closeWhenIdle();
}

const std::string& HttpHandler::get_server_name() const {
  const std::string& res = request_.getHeaders().getSingleOrEmpty(proxygen::HTTP_HEADER_HOST);
  if (res.empty()) {
    return url_.getHost();
  }

  return res;
}

void HttpHandler::SendRequest(proxygen::HTTPTransaction* txn) {
  txn_ = txn;
  request_.setMethod(http_method_);
  request_.setHTTPVersion(1, 1);
  request_.setURL(url_.makeRelativeURL());
  request_.setSecure(url_.isSecure());

  if (request_.getHeaders().getNumberOfValues(proxygen::HTTP_HEADER_USER_AGENT) == 0) {
    request_.getHeaders().add(proxygen::HTTP_HEADER_USER_AGENT, "proxygen_curl");
  }
  if (request_.getHeaders().getNumberOfValues(proxygen::HTTP_HEADER_HOST) == 0) {
    request_.getHeaders().add(proxygen::HTTP_HEADER_HOST, url_.getHostAndPort());
  }
  if (request_.getHeaders().getNumberOfValues(proxygen::HTTP_HEADER_ACCEPT) == 0) {
    request_.getHeaders().add("Accept", "*/*");
  }

  request_.dumpMessage(4);
  txn_->sendHeaders(request_);

  if (body_ != nullptr) {
    std::unique_ptr<folly::IOBuf> buf = folly::IOBuf::maybeCopyBuffer(*body_);
    if (buf != nullptr) {
      txn_->sendBody(move(buf));
    }
  }

  txn_->sendEOM();
}

void HttpHandler::connectError(const folly::AsyncSocketException& ex) {
  LOG(ERROR) << "Coudln't connect to "
             << url_.getHostAndPort() << ":" << ex.what();
}

void HttpHandler::setTransaction(proxygen::HTTPTransaction*) noexcept {
}

void HttpHandler::detachTransaction() noexcept {
}

void HttpHandler::onHeadersComplete(std::unique_ptr<proxygen::HTTPMessage> msg) noexcept {
  response_ = std::move(msg);

  std::cout << response_->getStatusCode() << " "
       << response_->getStatusMessage() << std::endl;
  response_->getHeaders().forEach([&](const std::string& header, const std::string& val) {
    std::cout << header << ": " << val << std::endl;
  });
}

void HttpHandler::onBody(std::unique_ptr<folly::IOBuf> chain) noexcept {
  if (resp_body_ == nullptr) {
    resp_body_.reset(new std::ostringstream());
  }

  if (chain != nullptr) {
    const folly::IOBuf* p = chain.get();
    do {
      resp_body_->write((const char*)p->data(), p->length());
      p = p->next();
    } while (p != chain.get());
  }
}

void HttpHandler::onTrailers(std::unique_ptr<proxygen::HTTPHeaders>) noexcept {
  LOG(INFO) << "Discarding trailers";
}

void HttpHandler::onEOM() noexcept {
  LOG(INFO) << "Got EOM";
  baton_->post();
}

void HttpHandler::onUpgrade(proxygen::UpgradeProtocol) noexcept {
  LOG(INFO) << "Discarding upgrade protocol";
}

void HttpHandler::onError(const proxygen::HTTPException& error) noexcept {
  LOG(ERROR) << "An error occurred: " << error.what();
  error_.reset(new proxygen::HTTPException(error));
  baton_->post();
}

void HttpHandler::onEgressPaused() noexcept {
  LOG(INFO) << "Egress paused";
}

void HttpHandler::onEgressResumed() noexcept {
  LOG(INFO) << "Egress resumed";
}

HttpResponse HttpHandler::MakeResponse() {
  HttpResponse response;

  CHECK(response_ != nullptr || error_ != nullptr);

  if (response_ != nullptr) {
    response.code = response_->getStatusCode();
    response.status_message = response_->getStatusMessage();

    response_->getHeaders().forEach([&](const std::string& header, const std::string& val) {
      response.headers[header] = val;
    });

    if (resp_body_ != nullptr) {
      response.body = resp_body_->str();
    }
  } else {
    response.code = 500;
    response.status_message = "INTERNAL_ERROR";
    response.body = error_->what();
  }

  return response;
}
}  // namespace

HttpClient::HttpClient(folly::EventBase* evb) : evb_(evb) {
}

HttpClient::~HttpClient() {
}

folly::HHWheelTimer* HttpClient::GetTimer() {
  if (timer_ != nullptr) {
    return timer_.get();
  }

  timer_ = folly::HHWheelTimer::newTimer(
      evb_,
      std::chrono::milliseconds(folly::HHWheelTimer::DEFAULT_TICK_INTERVAL),
      folly::AsyncTimeout::InternalEnum::NORMAL,
      std::chrono::milliseconds(5000));

  return timer_.get();
}

HttpResponse HttpClient::Post(const std::string& url,
                              const std::map<std::string, std::string> headers,
                              const std::string& body) {
  return SendWithBody(proxygen::HTTPMethod::POST, url, headers, body);
}

HttpResponse HttpClient::Patch(const std::string& url,
                               const std::map<std::string, std::string> headers,
                               const std::string& body) {
  // TODO(christian) put PATCH
  return SendWithBody(proxygen::HTTPMethod::POST, url, headers, body);
}

HttpResponse HttpClient::SendWithBody(proxygen::HTTPMethod method,
                                      const std::string& url_str,
                                      const std::map<std::string, std::string> headers_map,
                                      const std::string& body) {
  proxygen::URL url(url_str);
  proxygen::HTTPHeaders headers;
  for (const auto& it : headers_map) {
    headers.add(it.first, it.second);
  }

  folly::fibers::Baton baton; 
  HttpHandler handler(&baton, evb_, method, url, headers, &body);

  folly::SocketAddress addr(url.getHost(), url.getPort(), true);
  VLOG(1) << "Trying to connect to " << addr;

  proxygen::HTTPConnector connector(&handler, GetTimer());
  
  static const folly::AsyncSocket::OptionMap opts{{{SOL_SOCKET, SO_REUSEADDR}, 1}};

  if (url.isSecure()) {
    handler.InitializeSsl("h2,h2-14,spdy/3.1,spdy/3,http/1.1");
    connector.connectSSL(
        evb_,
        addr,
        handler.get_ssl_context(),
        nullptr,
        std::chrono::milliseconds(5000),
        opts,
        folly::AsyncSocket::anyAddress(),
        handler.get_server_name());
  } else {
    connector.connect(evb_, addr,
        std::chrono::milliseconds(5000), opts);
  }

  baton.wait();
  return handler.MakeResponse();                                        
}
}  // namespace tortuga
