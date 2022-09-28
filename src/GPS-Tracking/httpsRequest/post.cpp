#include "GPS-Tracking/httpsRequest/post.hpp"
#include "GPS-Tracking/database/database.hpp"

#include <nlohmann/json.hpp>
#include <iostream>

namespace karlo {
    namespace httpsRequest {

        using json = nlohmann::json;

        json getData(){
            json data = database::readData();
            //std::cout << data << std::endl;
            return data;
        }

        void post(std::string URL) {

            std::string imei = "5f105ae8a629de65677a0ce7"; //Temporarily using driver id
            std::string staticToken = "";

            httplib::Client cli(URL);

            std::string postUrl = "/api/tracking/last-location";

            json dataBody = getData();

            httplib::Params params;
                params.emplace("latitude", to_string(dataBody["latitude"]));
                params.emplace("longitude", to_string(dataBody["longitude"]));
                params.emplace("altitude", to_string(dataBody["altitude"]));
                params.emplace("speed", to_string(dataBody["speed"]));
                params.emplace("bearing", "100");
                params.emplace("driver", imei);
                //params.emplace("driver", static_cast<std::string>(dataBody["imei"]));

            auto res = cli.Post(postUrl, params);
            std::cout << "res : " << res->body << std::endl;
        }

    } // namespace httpRequest
} // namespace karlo
