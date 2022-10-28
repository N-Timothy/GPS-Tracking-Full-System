#ifndef _GPS_TRACKING_MQTT_CONNECTION_HPP_
#define _GPS_TRACKING_MQTT_CONNECTION_HPP_

#include <mosquitto.h>
#include <nlohmann/json.hpp>
#include <MQTTClient.h>

namespace karlo {
    namespace mqtt {

        using json = nlohmann::json;

        extern json config;

        void setMqttConfig(json mqttConfig);

    } // namespace database
} // namespace karlo


#endif //_GPS_TRACKING_MQTT_CONNECTION_HPP_
