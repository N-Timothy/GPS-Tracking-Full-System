#ifndef _GPS_TRACKING_MQTT_CONNECTION_HPP_
#define _GPS_TRACKING_MQTT_CONNECTION_HPP_

#include <nlohmann/json.hpp>
#include <MQTTAsync.h>

namespace karlo {
    namespace mqtt {

        using json = nlohmann::json;

        extern json config;

        void setMqttConfig(json mqttConfig);

    } // namespace database
} // namespace karlo


#endif //_GPS_TRACKING_MQTT_CONNECTION_HPP_
