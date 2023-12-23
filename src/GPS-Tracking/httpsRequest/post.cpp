#include "GPS-Tracking/httpsRequest/post.hpp"

#include <algorithm>
#include <condition_variable>
#include <iostream>
#include <iterator>
#include <mutex>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace karlo {
namespace httpsRequest {

std::mutex m;
std::condition_variable cv;

using namespace std::literals::chrono_literals;

using json = nlohmann::json;

std::map<int, std::pair<std::time_t, bool>> TIMEOUT;
void post(std::string URL, json config) {

  std::string staticToken = config["token"];
  std::string URL_STAGING = config["url_staging"];
  std::string battStatus = "\"Normal\"";

  try {

    httplib::Client cli(URL);
    httplib::Client cli_staging(URL_STAGING);

    cli.set_default_headers(
        {{"Authorization", staticToken}, {"Content-Type", "application/json"}});

    std::string postUrl = config["api"];

    std::unique_lock<std::mutex> lk(m);
    if (!cv.wait_until(lk, std::chrono::system_clock::now() + 3s,
                       [] { return ready; })) {
      // std::cout << "HTTPS TIMEOUT" << std::endl;
      return;
    } else {
      std::vector<json> postData = database::readData();

      for (json data : postData) {

        int tmp = ((float)data["latitude"] * 10000000);
        float latitude = (float)tmp / 10000000;

        tmp = ((float)data["longitude"] * 10000000);
        float longitude = (float)tmp / 10000000;

        int batt;
        data["exBattVoltage"].empty() ? batt = 0 : batt = data["exBattVoltage"];

        std::string status;
        if (data["ignitionOn"]) {
          if (to_string(data["speed"]) == "0") {
            status = "\"Idle\"";
          } else {
            status = "\"Moving\"";
          }
        } else {
          status = "\"Stop\"";
        }

        if (batt > 22500 && batt < 24500) {
          battStatus = "\"Warning\"";
        } else if (batt < 22500) {
          battStatus = "\"Low\"";
        } else if (batt == 0) {
          battStatus = "\"Unplugged\"";
        }

        std::string Msg = "{\"timeCreated\":" + to_string(data["timestamp"]) +
                          "," + "\"latitude\":" + std::to_string(latitude) +
                          "," + "\"longitude\":" + std::to_string(longitude) +
                          "," + "\"altitude\":" + std::to_string(0) + "," +
                          "\"speed\":" + to_string(data["speed"]) + "," +
                          "\"bearing\":" + to_string(data["bearing"]) + "," +
                          "\"imeiTracker\":" + to_string(data["imei"]) + "," +
                          "\"battStatus\":" + battStatus + "," +
                          "\"status\":" + status + "}";

        auto res = cli.Post(postUrl, Msg, "application/json");
        auto res_staging = cli_staging.Post(postUrl, Msg, "application/json");

        if (res) {
          std::cout << "production: " << res->body << std::endl;
        }

        if (res_staging) {
          std::cout << "staging: " << res->body << std::endl;
        }
      }
    }
  } catch (...) {
    std::cout << "HTTP FAILED" << std::endl;
  }
}

void post(std::string URL, json config, json data) {

  std::string staticToken = config["token"];
  std::string URL_STAGING = config["url_staging"];
  std::string URL_BETA = config["url_beta"];
  std::string URL_GPS_BACKEND = config["url_gps_backend"];
  std::string battStatus = "\"Normal\"";

  httplib::Client cli(URL);
  httplib::Client cli_staging(URL_STAGING);
  httplib::Client cli_beta(URL_BETA);
  httplib::Client cli_gps_backend(URL_GPS_BACKEND);

  cli.set_default_headers(
      {{"Authorization", staticToken}, {"Content-Type", "application/json"}});

  std::string postUrl = config["api"];

  int batt;
  data["exBattVoltage"].empty() ? batt = 0 : batt = data["exBattVoltage"];

  std::string status;
  if (data["ignitionOn"]) {
    if (to_string(data["speed"]) == "0") {
      status = "\"Idle\"";
    } else {
      status = "\"Moving\"";
    }
  } else {
    status = "\"Stop\"";
  }

  if (batt < 24500 && batt > 22500) {
    battStatus = "\"Warning\"";
  } else if (batt < 22500) {
    battStatus = "\"Low\"";
  } else if (batt == 0) {
    battStatus = "\"Unplugged\"";
  }

  std::string Msg = "{\"timeCreated\":" + to_string(data["timestamp"]) + "," +
                    "\"latitude\":" + to_string(data["latitude"]) + "," +
                    "\"longitude\":" + to_string(data["longitude"]) + "," +
                    "\"altitude\":" + to_string(data["altitude"]) + "," +
                    "\"speed\":" + to_string(data["speed"]) + "," +
                    "\"bearing\":" + to_string(data["bearing"]) + "," +
                    "\"imeiTracker\":" + to_string(data["imei"]) + "," +
                    "\"battStatus\":" + battStatus + "," +
                    "\"status\":" + status + "}";

  std::string MsgBackend =
      "{\"imei\":" + to_string(data["imei"]) + "," +
      "\"driver\":" + "\"none\"" + "," +
      "\"ignitionOn\":" + to_string(data["ignitionOn"]) + "," +
      "\"latitude\":" + to_string(data["latitude"]) + "," +
      "\"longitude\":" + to_string(data["longitude"]) + "," +
      "\"altitude\":" + to_string(data["altitude"]) + "," +
      "\"speed\":" + to_string(data["speed"]) + "," +
      "\"bearing\":" + to_string(data["bearing"]) + "," +
      "\"sleepMode\":" + "0" + "," +
      "\"exBattVoltage\":" + to_string(data["exBattVoltage"]) + "," +
      "\"battStatus\":" + battStatus + "," + "\"status\":" + status + "," +
      "\"description\":" + "\"This is default value\"" + "," +
      "\"truck\":" + "\"none\"" + "," + "\"city\":" + "0" + "," +
      "\"createdAt\":" + to_string(data["timestamp"]) + "," + "\"_v\":" + "0" +
      "}";

  // auto res = cli.Post(postUrl, Msg, "application/json");
  // auto res_staging = cli_staging.Post(postUrl, Msg, "application/json");
  auto res_gps_backend = cli_gps_backend.Post("/gps/last-location", MsgBackend,
                                              "application/json");

  std::string imei = to_string(data["imei"]);

  // auto res_beta = cli_beta.Post(postUrl, Msg, "application/json");
  // if (res_beta) {
  //   std::cout << "beta: " << res_beta->body << std::endl;
  // }

  // if (res) {
  //   std::cout << "production: " << data["imei"] << std::endl;
  // }

  // if (res_staging) {
  //   std::cout << "staging: " << data["imei"] << std::endl;
  // }

  if (res_gps_backend) {
    //   std::cout << "gps_backend: " << res_gps_backend->body << std::endl;
    std::cout << "gps_backend: " << data["imei"] << std::endl;
  }
}

} // namespace httpsRequest
} // namespace karlo
