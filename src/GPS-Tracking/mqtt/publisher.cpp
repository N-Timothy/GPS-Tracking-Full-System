#include "GPS-Tracking/mqtt/publisher.hpp"
#include "GPS-Tracking/database/database.hpp"

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

            std::unique_lock<std::mutex> lk(m);
            cv.wait(lk, []{return ready;});

            json data = database::readData(imei);
            if (data.is_null()){
                return -1;
            }

            json publishMessage;
            publishMessage["latitude"] = to_string(data["latitude"]);
            publishMessage["longitude"] = to_string(data["longitude"]);
            publishMessage["altitude"] = to_string(data["altitude"]);
            publishMessage["speed"] = to_string(data["speed"]);
            publishMessage["bearing"] = "100";
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
