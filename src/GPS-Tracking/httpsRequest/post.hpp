#ifndef _GPS_TRACKING_HTTPS_POST_HPP_
#define _GPS_TRACKING_HTTPS_POST_HPP_

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"

#include "GPS-Tracking/database/database.hpp"

#include <string>
namespace karlo {
    namespace httpsRequest {
        
        using json = nlohmann::json;

        void post(std::string URL, json config);

        void post(std::string URL, json config, json data);

    } // namespace httpRequest
} // namespace karlo

#endif // _GPS_TRACKING_HTTPS_POST_HPP_
