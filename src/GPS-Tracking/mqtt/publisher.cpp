#include "GPS-Tracking/mqtt/publisher.hpp"
#include "GPS-Tracking/database/database.hpp"
#include "GPS-Tracking/common/timer.hpp"

#include <iostream>
#include <string>
#include <mutex>
#include <condition_variable>

namespace karlo { 
    namespace mqtt {

        std::mutex m;
        std::condition_variable cv;

        using json = nlohmann::json;
        int publisher(std::string imei) {

	        int rc;
	        struct mosquitto * mosq;

	        mosquitto_lib_init();

	        mosq = mosquitto_new("Karlo-tracker-MQTT-Publisher", true, NULL);

            std::string host = config["host"];

	        rc = mosquitto_connect(mosq, host.c_str(), config["port"], 60);
	        if(rc != 0){
                std::cout << "Client could not connect to broker! Error Code: " << rc << std::endl;
		        mosquitto_destroy(mosq);
		        return -2;
	        }
	        std::cout << "We are now connected to the broker! " << std::endl;

            //  using -2 as mqtt id
            common::add_timeout(-2);

            std::unique_lock<std::mutex> lk(m);
            //cv.wait(lk, []{return ready || common::TIMEOUT[-2].second;});
            cv.wait(lk, []{return common::TIMEOUT[-2].second;});

            json data = database::readData(imei);

            common::delete_timeout(-2);
            if (data.is_null() || common::TIMEOUT[-2].second) {
                return -1;
            }

            int tmp = ((float) data["latitude"] * 10000000);
            float latitude = (float) tmp / 10000000;

            tmp = ((float) data["longitude"] * 10000000);
            float longitude = (float) tmp / 10000000;

            json publishMessage;
            publishMessage["latitude"] = std::to_string(latitude);
            publishMessage["longitude"] = std::to_string(longitude);
            publishMessage["altitude"] = to_string(data["altitude"]);
            publishMessage["speed"] = to_string(data["speed"]);
            publishMessage["bearing"] = to_string(data["bearing"]);
            publishMessage["driverid"] = data["imei"];

            std::cout << "msg : " << publishMessage << std::endl;

            std::string pub_topic = config["pub_topic"];

	        mosquitto_publish(mosq, NULL, pub_topic.c_str(), 250 ,publishMessage.dump().c_str(), 0, false);

	        mosquitto_disconnect(mosq);
	        mosquitto_destroy(mosq);

	        mosquitto_lib_cleanup();
            
            return 0;
        }

    } // namespace mqtt

} // namespace karlo
