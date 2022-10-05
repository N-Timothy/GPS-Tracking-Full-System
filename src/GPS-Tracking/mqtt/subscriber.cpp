#include "GPS-Tracking/mqtt/subscriber.hpp"
#include "GPS-Tracking/mqtt/toggleController.hpp"

#include <string>

namespace karlo {
    namespace mqtt {

        using json = nlohmann::json;

        void on_connect(struct mosquitto *mosq, void *obj, int rc) {
            std::cout << "ID: " << * (int *) obj << std::endl;
	        if(rc) {
                std::cout << "Error with result code: " << rc << std::endl;
		        exit(-1);
	        }
	        mosquitto_subscribe(mosq, NULL, MQTT_SUB_TOPIC, 0);
        }

        void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) {
            json Message = json::parse((char *) msg->payload);

            toggleController(Message);
         }

        void subscriber() {

	        int rc, id=12;

	        mosquitto_lib_init();

	        struct mosquitto *mosq;

	        mosq = mosquitto_new("Karlo-tracker-MQTT-Subscriber" , true, &id);
	        mosquitto_connect_callback_set(mosq, on_connect);
	        mosquitto_message_callback_set(mosq, on_message);
	
        	rc = mosquitto_connect(mosq, MQTT_HOST, MQTT_PORT, 10);
        	if(rc) {
                std::cout << "Could not connect to Broker with return code " << rc << std::endl;
		        return ;
	        }

	        mosquitto_loop_start(mosq);
            std::cout << "Press Enter to quit...." << std::endl;
	        getchar();
	        mosquitto_loop_stop(mosq, true);

	        mosquitto_disconnect(mosq);
	        mosquitto_destroy(mosq);
	        mosquitto_lib_cleanup();

        }
        
    } // namespace mqtt
} // namespace karlo
