#ifndef _GPS_TRACKING_MQTT_PUBLISHER_HPP_
#define _GPS_TRACKING_MQTT_PUBLISHER_HPP_
#include "GPS-Tracking/mqtt/connection.hpp"

#include <string>

namespace karlo {
    namespace mqtt {

        extern bool pub_ready;
        extern std::string null_imei;

        void publisher(std::string _imei);

    } // namespace mqtt
} // namespace karlo
#endif // _GPS_TRACKING_MQTT_PUBLISHER_HPP_
