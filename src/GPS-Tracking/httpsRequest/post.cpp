#include "GPS-Tracking/httpsRequest/post.hpp"

#include <nlohmann/json.hpp>
#include <iostream>
#include <vector>
#include <mutex>
#include <condition_variable>

namespace karlo {
    namespace httpsRequest {

        std::mutex m;
        std::condition_variable cv;
        
        using namespace std::literals::chrono_literals;

        using json = nlohmann::json;

        std::map<int, std::pair<std::time_t, bool>> TIMEOUT;

        void post(std::string URL, json config) {

            std::string staticToken = config["token"];

            httplib::Client cli(URL);
            
            cli.set_default_headers({ { "Authorization", staticToken } });

            std::string postUrl = config["api"];

            std::unique_lock<std::mutex> lk(m);
            if(!cv.wait_until(lk, std::chrono::system_clock::now() + 3s, []{return ready;})) {
                //std::cout << "HPPTS TIMEOUT" << std::endl;
                return;
            } else {

                std::vector<json> postData = database::readData();

                for (json data : postData) {

                    int tmp = ((float) data["latitude"] * 10000000);
                    float latitude = (float) tmp / 10000000;

                    tmp = ((float) data["longitude"] * 10000000);
                    float longitude = (float) tmp / 10000000;

                    std::string batt;
                    data["exBattVoltage"].empty() ? batt = std::to_string(0) : batt = to_string(data["exBattVoltage"]);

                    json stat;
                    if(data["ignitionOn"]) {
                        if(to_string(data["speed"]) == "0"){
                            stat = json::parse(R"({"status": "idle"})");
                        } else {
                            stat = json::parse(R"({"status": "moving"})");
                        }
                    } else {
                            stat = json::parse(R"({"status": "stop"})");
                    }

                    std::cout << "status : " << stat["status"] << std::endl;

                    httplib::Params params;
                        params.emplace("latitude", std::to_string(latitude));
                        params.emplace("longitude", std::to_string(longitude));
                        params.emplace("altitude", to_string(data["altitude"]));
                        params.emplace("speed", to_string(data["speed"]));
                        params.emplace("bearing", to_string(data["bearing"]));
                        params.emplace("imeiTracker", data["imei"]);
                        params.emplace("battery", batt);
                        params.emplace("status", stat["status"]);
                    auto res = cli.Post(postUrl, params);

                    if (res) {
                        std::cout << res->body << std::endl;
                    }
                }
            }
        }

    } // namespace httpRequest
} // namespace karlo
