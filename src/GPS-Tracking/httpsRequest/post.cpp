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

        using json = nlohmann::json;

        void post(std::string URL, json config) {

            std::string imei = "5f105ae8a629de65677a0ce7"; //Temporarily using driver id
            std::string staticToken = config["token"];

            httplib::Client cli(URL);

            std::string postUrl = config["api"];

            std::unique_lock<std::mutex> lk(m);
            cv.wait(lk, []{return ready;});

            std::vector<json> postData = database::readData();

            for (json data : postData) {

              httplib::Params params;
                    params.emplace("latitude", to_string(data["latitude"]));
                    params.emplace("longitude", to_string(data["longitude"]));
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
