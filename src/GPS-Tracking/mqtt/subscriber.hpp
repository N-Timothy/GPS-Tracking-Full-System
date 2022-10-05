#ifndef _GPS_TRACKING_MQTT_CLIENT_HPP_
#define _GPS_TRACKING_MQTT_CLIENT_HPP_
#include "GPS-Tracking/mqtt/connection.hpp"

#include <mosquitto.h>
#include <iostream>

namespace karlo {
    namespace mqtt {

        void subscriber();

    } // namesapce mqtt
} // namespace karlo

#endif // _GPS_TRACKING_MQTT_CLIENT_HPP_
