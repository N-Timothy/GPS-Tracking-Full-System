#ifndef _GPS_TRACKING_CORE_CONFIG_HPP_
#define _GPS_TRACKING_CORE_CONFIG_HPP_

#define CONFIG_DIR ".config/tracker/config.json"

#include <nlohmann/json.hpp>

namespace karlo {
    namespace core {
        namespace config {

            using json = nlohmann::json;

            void config();

        } // namespace config
    } // namespace core
} // namespace karlo

#endif // _GPS_TRACKING_CORE_CONFIG_HPP_
