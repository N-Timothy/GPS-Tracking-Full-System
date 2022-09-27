#include "GPS-Tracking/httpsRequest/httpsRequest.hpp"
#include "GPS-Tracking/httpsRequest/post.hpp"

#include <iostream>
#include <string>

namespace karlo {
    namespace httpsRequest {

        void connect() {
            std::string URL = "https://server-staging.karlo.id";

            post(URL);
        }

    } // namespace httpRequest
} //namespace karlo
