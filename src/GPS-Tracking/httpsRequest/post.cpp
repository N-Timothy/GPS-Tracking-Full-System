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
            std::string battStatus = "\"OK\"";

            httplib::Client cli(URL);
            
            cli.set_default_headers({ { "Authorization", staticToken }, {"Content-Type","application/json"} });

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

                    int batt;
                    data["exBattVoltage"].empty() ? batt = 0 : batt = data["exBattVoltage"];

                    std::string status;
                    if(data["ignitionOn"]) {
                        if(to_string(data["speed"]) == "0"){
                            status = "\"idle\"";
                        } else {
                            status = "\"moving\"";
                        }
                    } else {
                        status = "\"stop\"";
                    }

                    if(batt < 24500 && batt > 22500) {
                        battStatus = "\"WARNING\"";
                    } else if (batt < 22500) {
                        battStatus = "\"LOW\"";
                    }

                    std::string Msg = "{\"latitude\":" + std::to_string(latitude) + "," + "\"longitude\":" + std::to_string(longitude) + "," + "\"altitude\":" + to_string(data["altitude"]) + "," + "\"speed\":" + to_string(data["speed"]) + "," + "\"bearing\":" + to_string(data["bearing"]) + "," + "\"imeiTracker\":" + to_string(data["imei"]) + "," + "\"battStatus\":" + battStatus + "," + "\"status\":" + status + "}";
                    
                    auto res = cli.Post(postUrl, Msg , "application/json");

                    if (res) {
                        std::cout << res->body << std::endl;
                    }
                }
            }
        }

    } // namespace httpRequest
} // namespace karlo
