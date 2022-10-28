#include "GPS-Tracking/mqtt/subscriber.hpp"
#include "GPS-Tracking/mqtt/toggleController.hpp"

#define CLIENTID "GPS-Tracker-Sub"

#include <string>
#include <unistd.h>

namespace karlo {
    namespace mqtt {

        using json = nlohmann::json;

        json Message;

        //void on_connect(struct mosquitto *mosq, void *obj, int rc) {
          //  std::cout << "ID: " << * (int *) obj << std::endl;
	      //  if(rc) {
          //      std::cout << "Error with result code: " << rc << std::endl;
		  //      exit(-1);
	      //  }

            //std::string sub_topic = config["sub_topic"];

	        //mosquitto_subscribe(mosq, NULL, sub_topic.c_str(), 0);
       // }

       // void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) {
       //     json Message = json::parse((char *) msg->payload);

        //    toggleController(Message);
        // }


        volatile MQTTClient_deliveryToken deliveredTokenSub;

        void delivered(void *context, MQTTClient_deliveryToken dt)
        {
            printf("Message with token value %d delivery confirmed\n", dt);
            deliveredTokenSub = dt;
        }
        
        int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
        {
            int i;
            char* payloadptr;
            printf("Message arrived\n");
            printf("     topic: %s\n", topicName);
            printf("   message: ");
            payloadptr = (char*) message->payload;
            Message = json::parse((char*) message->payload);

            MQTTClient_freeMessage(&message);
            MQTTClient_free(topicName);

            return 1;
        }
        
        void connlost(void *context, char *cause)
        {
            printf("\nConnection lost\n");
            printf("     cause: %s\n", cause);
        }

        void subscriber() {

    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    int rc;

    std::string host = config["host"];
    std::string port = to_string(config["port"]);
    std::string endpoint = host + ":" + port;
    std::cout << "endpoint : " << endpoint.c_str() << std::endl;
    std::string sub_topic = config["sub_topic"];
    int QOS = config["QOS"];

    MQTTClient_create(&client, host.c_str(), CLIENTID,
        MQTTCLIENT_PERSISTENCE_NONE, NULL);
//    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        return;
    }

    MQTTClient_subscribe(client, sub_topic.c_str(), QOS);
    std::cout << "message : " << Message << std::endl;

    toggleController(Message);

    for(;;){}
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);

	        //int rc, id=12;

	        //mosquitto_lib_init();

	        //struct mosquitto *mosq;

	       // mosq = mosquitto_new("Karlo-tracker-MQTT-Subscriber" , true, &id);
	       // mosquitto_connect_callback_set(mosq, on_connect);
	       // mosquitto_message_callback_set(mosq, on_message);

           // std::string host = config["host"];
	
        //	rc = mosquitto_connect(mosq, host.c_str(), config["port"], 10);
      //  	if(rc) {
       //         std::cout << "Could not connect to Broker with return code " << rc << std::endl;
		//        return ;
	     //   }

	      //  mosquitto_loop_start(mosq);
           // std::cout << "Press Enter to quit...." << std::endl;
	       // getchar();
	       // mosquitto_loop_stop(mosq, true);

	      //  mosquitto_disconnect(mosq);
	      //  mosquitto_destroy(mosq);
	      //  mosquitto_lib_cleanup();

        }
        
    } // namespace mqtt
} // namespace karlo
