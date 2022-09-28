#ifndef _GPS_TRACKING_MQTT_TOGGLE_CONTROLLER_HPP_
#define _GPS_TRACKING_MQTT_TOGGLE_CONTROLLER_HPP_

#include "GPS-Tracking/mqtt/connection.hpp"

namespace karlo {
    namespace mqtt {

        using json = nlohmann::json;

        void toggleController(json subscribeMessage);

    } // namespace mqtt
} // anemsapce karlo

#endif // _GPS_TRACKING_MQTT_TOGGLE_CONTROLLER_HPP_

