#ifndef _GPS_TRACKING_HTTPS_REQUEST_HPP_
#define _GPS_TRACKING_HTTPS_REQUEST_HPP_

/*
    #define CPPHTTPLIB_OPENSSL_SUPPORT is required for https
    this need to be placed before #include "httplib.h" 
    or it will produce segmentation fault
*/

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"

#include <nlohmann/json.hpp>


namespace karlo {
    namespace httpsRequest {
        
        using json = nlohmann::json;

        void setHttpsConfig(json httpsConfig);

        void connect();
        
        void singleConnect(json data);
    
    } // namespace httpRequest
} // namespace karlo
#endif //_GPS_TRACKING_HTTPS_REQUEST_HPP_
