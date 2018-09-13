#include <memory>
#include <string>
#include <thread>

#include "folly/fibers/FiberManager.h"
#include "folly/fibers/FiberManagerMap.h"
#include "folly/io/async/EventBase.h"
#include "folly/init/Init.h"
#include "gflags/gflags.h"
#include "glog/logging.h"

#include "tortuga/http.h"

int main(int argc, char** argv) {
  folly::init(&argc, &argv, true);
  
  folly::EventBase evb;
  folly::fibers::FiberManager::Options fiber_opts;
  fiber_opts.stackSize = 1024 * 1024;
  folly::fibers::FiberManager& fibers = folly::fibers::getFiberManager(evb, fiber_opts);
  
  fibers.addTask([&evb]() {
    tortuga::HttpClient client(&evb);
    tortuga::HttpResponse resp = client.Post("https://postman-echo.com/post", {}, "The test!");
    LOG(INFO) << "response code: " << resp.code;
    LOG(INFO) << "response body: " << resp.body;

    LOG(FATAL) << "we are done, bye bye!";
  });

  evb.loopForever();
  return 0;
}
