#include "GPS-Tracking/mqtt/connection.hpp"

namespace karlo {
    namespace mqtt {
        
        using json = nlohmann::json;

        json config;

        void setMqttConfig(json mqttConfig){
            config = mqttConfig;
        }
    } // namespace mqtt
} // namespace karlo


