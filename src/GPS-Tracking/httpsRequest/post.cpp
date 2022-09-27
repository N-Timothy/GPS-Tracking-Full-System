#include "GPS-Tracking/httpsRequest/post.hpp"
#include "GPS-Tracking/database/database.hpp"

#include <nlohmann/json.hpp>
#include <iostream>

namespace karlo {
    namespace httpsRequest {

        using json = nlohmann::json;

        json postData(){
            json data = database::readData();
            std::cout << data << std::endl;
            return data;
        }

        void post(std::string URL) {

            std::string imei = "5f105ae8a629de65677a0ce7"; //Temporarily using driver id
            std::string staticToken = "";

            httplib::Client cli(URL);

            std::string postUrl = "/api/tracking/last-location";

            httplib::Params params;
                params.emplace("latitude", "-6.37373");
                params.emplace("longitude", "107.482683");
                params.emplace("altitude", "100");
                params.emplace("speed", "100");
                params.emplace("bearing", "100");
                params.emplace("driver", imei);

            auto res = cli.Post(postUrl, params);
            std::cout << "res : " << res->body << std::endl;
        }

    } // namespace httpRequest
} // namespace karlo
