#pragma once

#include <map>
#include <memory>
#include <string>

#include "folly/io/async/EventBase.h"
#include "folly/io/async/HHWheelTimer.h"
#include "proxygen/lib/http/HTTPMethod.h"

namespace tortuga {
struct HttpResponse {
  int code;
  std::string status_message;
  std::string body;
  std::map<std::string, std::string> headers;
};

// client shall be reused.
class HttpClient {
 public:
   // 'evb' not owned.
  explicit HttpClient(folly::EventBase* evb);
  ~HttpClient();

  HttpResponse Post(const std::string& url,
                    const std::map<std::string, std::string> headers,
                    const std::string& body);

  HttpResponse Patch(const std::string& url,
                     const std::map<std::string, std::string> headers,
                     const std::string& body);

 private:
  HttpResponse SendWithBody(proxygen::HTTPMethod method,
                            const std::string& url,
                            const std::map<std::string, std::string> headers,
                            const std::string& body);

  folly::HHWheelTimer* GetTimer();

  folly::EventBase* evb_{ nullptr };
  folly::HHWheelTimer::UniquePtr timer_;
};
}  // namespace tortuga
