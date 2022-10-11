#include "GPS-Tracking/httpsRequest/post.hpp"
#include "GPS-Tracking/common/timer.hpp"

#include <nlohmann/json.hpp>
#include <iostream>
#include <vector>
#include <mutex>
#include <condition_variable>

namespace karlo {
    namespace httpsRequest {

        std::mutex m;
        std::condition_variable cv;

        using json = nlohmann::json;

        void post(std::string URL, json config) {

            std::string imei = "5f105ae8a629de65677a0ce7"; //Temporarily using driver id
            std::string staticToken = config["token"];

            httplib::Client cli(URL);
            
            cli.set_default_headers({ { "Authorization", staticToken } });

            std::string postUrl = config["api"];

            common::add_timeout(-1);

            std::unique_lock<std::mutex> lk(m);
            //cv.wait(lk, []{return ready || common::TIMEOUT[-1].second;});
            cv.wait(lk, []{return common::TIMEOUT[-1].second;});

            if (common::TIMEOUT[-1].second){
                std::cout << "HPPTS TIMEOUT" << std::endl;
                return;
            }

            std::vector<json> postData = database::readData();

            common::delete_timeout(-1);

            for (json data : postData) {

              int tmp = ((float) data["latitude"] * 10000000);
              float latitude = (float) tmp / 10000000;

              tmp = ((float) data["longitude"] * 10000000);
              float longitude = (float) tmp / 10000000;

              httplib::Params params;
                    params.emplace("latitude", std::to_string(latitude));
                    params.emplace("longitude", std::to_string(longitude));
                    params.emplace("altitude", to_string(data["altitude"]));
                    params.emplace("speed", to_string(data["speed"]));
                    params.emplace("bearing", "100");
                    params.emplace("driver", imei);
                    //params.emplace("driver", to_string(data["imei"]));
                auto res = cli.Post(postUrl, params);

                if (res) {
                    std::cout << res->body << std::endl;
                }
            }

        }

    } // namespace httpRequest
} // namespace karlo
