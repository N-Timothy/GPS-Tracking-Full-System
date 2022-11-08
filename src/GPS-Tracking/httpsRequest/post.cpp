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

                    // ------- Status ---------
                    std::string status;
    
                    if(data["ignitionOn"]){
                        if (data["speed"] != 0){
                            status = "moving";
                        } else {
                            status = "idle";
                        }
                    } else {
                        status = "stop";
                    }
                    std::cout << "Status : " << status << std::endl;
                    // ------- Status ---------

                    int tmp = ((float) data["latitude"] * 10000000);
                    float latitude = (float) tmp / 10000000;

                    tmp = ((float) data["longitude"] * 10000000);
                    float longitude = (float) tmp / 10000000;

                    int batt;

                    httplib::Params params;
                        //params.emplace("latitude", std::to_string(latitude));
                        //params.emplace("longitude", std::to_string(longitude));
                        //params.emplace("altitude", std::to_string((int)data["altitude"]));
                        //params.emplace("speed", std::to_string((int)data["speed"]));
                        //params.emplace("bearing", std::to_string((int)data["bearing"]));
                        //params.emplace("imeiTracker", to_string(data["imei"]));
                        //params.emplace("battery", std::to_string(batt));
                        //params.emplace("status", status);

                        params.emplace("latitude", "1");
                        params.emplace("longitude", "2");
                        params.emplace("altitude", std::to_string("3");
                        params.emplace("speed", std::to_string("4");
                        params.emplace("bearing", std::to_string("5");
                        params.emplace("imeiTracker", to_string("6");
                        params.emplace("battery", std::to_string("7"));
                        params.emplace("status", status);
                    auto res = cli.Post(postUrl, params);

                    if (res) {
                        std::cout << res->body << std::endl;
                    }
                }
            }
        }

    } // namespace httpRequest
} // namespace karlo
