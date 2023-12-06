#include "GPS-Tracking/httpsRequest/httpsRequest.hpp"
#include "GPS-Tracking/core/config.hpp"
#include "GPS-Tracking/httpsRequest/post.hpp"

#include <chrono>
#include <iostream>
#include <string>

namespace karlo {
namespace httpsRequest {

json config;

void setHttpsConfig(json httpsConfig) { config = httpsConfig; }

void connect() {

  // json config = core::config::config_https();
  std::string URL = config["url"];
  int timeInterval = config["interval"];

  while (true) {
    std::this_thread::sleep_for(std::chrono::seconds(timeInterval));
    post(URL, config);
  }
}

void singleConnect(json data) {
  std::string URL = config["url"];

  post(URL, config, data);
}

} // namespace httpsRequest
} // namespace karlo
