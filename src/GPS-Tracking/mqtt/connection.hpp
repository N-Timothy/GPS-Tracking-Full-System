#ifndef _GPS_TRACKING_MQTT_CONNECTION_HPP_
#define _GPS_TRACKING_MQTT_CONNECTION_HPP_

#define MQTT_HOST "localhost"
#define MQTT_SUB_TOPIC "tracker/sub"
#define MQTT_PUB_TOPIC "tracker/pub"
#define MQTT_PORT 1883

#include <mosquitto.h>
#include <nlohmann/json.hpp>


#endif //_GPS_TRACKING_MQTT_CONNECTION_HPP_
