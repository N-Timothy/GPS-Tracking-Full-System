#include "GPS-Tracking/mqtt/publisher.hpp"
#include "GPS-Tracking/database/database.hpp"

#define CLIENTID "GPS-Tracker-Pub"

#include <iostream>
#include <string>
#include <mutex>
#include <condition_variable>

namespace karlo { 
    namespace mqtt {

        std::mutex m;
        std::condition_variable cv;

        using namespace std::literals::chrono_literals;

        using json = nlohmann::json;

        int publisher(std::string imei) {

            MQTTClient client;
            MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
            MQTTClient_message pubmsg = MQTTClient_message_initializer;
            MQTTClient_deliveryToken token;
            int rc;

            std::string host = config["host"];
            std::string port = to_string(config["port"]);
            std::string endpoint = host + ":" + port;
            std::string pub_topic = config["pub_topic"];
            std::cout << "endpoint : " << endpoint.c_str() << std::endl;
    
            MQTTClient_create(&client, endpoint.c_str(), CLIENTID,
            MQTTCLIENT_PERSISTENCE_NONE, NULL);
            conn_opts.keepAliveInterval = 20;
            conn_opts.cleansession = 1;
            if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
            {
                printf("Failed to connect, return code %d\n", rc);
                return -2;
            }

	        //int rc;
	        //struct mosquitto * mosq;

	        //mosquitto_lib_init();

	        //mosq = mosquitto_new("Karlo-tracker-MQTT-Publisher", true, NULL);

            //std::string host = config["host"];

	        //rc = mosquitto_connect(mosq, host.c_str(), config["port"], 60);
	        //if(rc != 0){
            //    std::cout << "Client could not connect to broker! Error Code: " << rc << std::endl;
		    //    mosquitto_destroy(mosq);
		    //    return -2;
	        //}

	        std::cout << "We are now connected to the broker! " << std::endl;

            std::unique_lock<std::mutex> lk(m);
            if(!cv.wait_until(lk, std::chrono::system_clock::now() + 3s, []{return ready;})){
                std::cout << "timeout" << std::endl;
                return -3;
            } else {

                json data = database::readData(imei);
                if (data.is_null()){
                    return -1;
                }

                int tmp = ((float) data["latitude"] * 10000000);
                float latitude = (float) tmp / 10000000;

                tmp = ((float) data["longitude"] * 10000000);
                float longitude = (float) tmp / 10000000;

                json PAYLOAD;
                PAYLOAD["latitude"] = std::to_string(latitude);
                PAYLOAD["longitude"] = std::to_string(longitude);
                PAYLOAD["altitude"] = to_string(data["altitude"]);
                PAYLOAD["speed"] = to_string(data["speed"]);
                PAYLOAD["bearing"] = to_string(data["bearing"]);
                PAYLOAD["imeiTracker"] = data["imei"];

                std::cout << "msg : " << PAYLOAD << std::endl;

                pubmsg.payload = (void*) PAYLOAD.dump().c_str();
                pubmsg.payloadlen = strlen(PAYLOAD.dump().c_str());
                pubmsg.qos = config["QOS"];
                pubmsg.retained = 0;

                MQTTClient_publishMessage(client, pub_topic.c_str(), &pubmsg, &token);
                MQTTClient_disconnect(client, 10000);
                MQTTClient_destroy(&client);

	            //mosquitto_publish(mosq, NULL, pub_topic.c_str(), 250 ,publishMessage.dump().c_str(), 0, false);

	            //mosquitto_disconnect(mosq);
	            //mosquitto_destroy(mosq);

	            //mosquitto_lib_cleanup();
            }

            return 0;
        }

    } // namespace mqtt

} // namespace karlo
