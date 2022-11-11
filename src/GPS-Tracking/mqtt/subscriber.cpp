#include "GPS-Tracking/mqtt/subscriber.hpp"
#include "GPS-Tracking/mqtt/toggleController.hpp"

#define CLIENTID "GPS-Tracker-Sub"

#include <string>
#include <unistd.h>

namespace karlo {
    namespace mqtt {

        using json = nlohmann::json;

        json Message;

        int disc_finished = 0;
        int subscribed = 0;
        int finished = 0;

        void connlost(void *context, char *cause)

        {
        
            MQTTAsync client = (MQTTAsync)context;
       
            MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
        
            int rc;
 

            printf("\nConnection lost\n");
        
            if (cause) {
                printf("     cause: %s\n", cause); 
            }
 
            printf("Reconnecting\n");
            conn_opts.keepAliveInterval = 20;
            conn_opts.cleansession = 1;
            if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS)
            {
                printf("Failed to start connect, return code %d\n", rc);
                finished = 1;
            }

        }
 
        int msgarrvd(void *context, char *topicName, int topicLen, MQTTAsync_message *message)
        {
   
            char* payloadptr;
            json msgJson;

            printf("Message arrived\n");
            printf("     topic: %s\n", topicName);
            printf("   message: %.*s\n", message->payloadlen, (char*)message->payload);

	        payloadptr = (char*)message->payload;
            std::string msg = "";
        	for (int i = 0; i<message->payloadlen; i++)
        	{
        		msg = msg + *payloadptr++;
        	}
            try {
                msgJson = json::parse(msg);
            } catch (json::parse_error& e){
                std::cout << "wrong format" << std::endl;
            }
    
            MQTTAsync_freeMessage(&message);
            MQTTAsync_free(topicName);
            toggleController(msgJson);
            return 1;
        }

        void onDisconnectFailure(void* context, MQTTAsync_failureData* response)

        {
            printf("Disconnect failed, rc %d\n", response->code);
            disc_finished = 1;
        }
 

        void onDisconnect(void* context, MQTTAsync_successData* response)

        {
            printf("Successful disconnection\n");
            disc_finished = 1;
        }
 

        void onSubscribe(void* context, MQTTAsync_successData* response)
        {
            printf("Subscribe succeeded\n");
            subscribed = 1;
        }
 

        void onSubscribeFailure(void* context, MQTTAsync_failureData* response)
       {
            printf("Subscribe failed, rc %d\n", response->code);
            finished = 1;
        }
 
 
        void onConnectFailure(void* context, MQTTAsync_failureData* response)
        {
            printf("Connect failed, rc %d\n", response->code);
            finished = 1;
        }
 
 

        void onConnect(void* context, MQTTAsync_successData* response)
        {

            std::string sub_topic = config["sub_topic"];
            int QOS = config["QOS"];

            MQTTAsync client = (MQTTAsync)context;
            MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
            int rc;
            printf("Successful connection\n");
            opts.onSuccess = onSubscribe;
            opts.onFailure = onSubscribeFailure;
            opts.context = client;
            if ((rc = MQTTAsync_subscribe(client, sub_topic.c_str(), QOS, &opts)) != MQTTASYNC_SUCCESS)
            {
                printf("Failed to start subscribe, return code %d\n", rc);
                finished = 1;
            }
        }


void subscriber() {

    MQTTAsync client;
    MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
    MQTTAsync_disconnectOptions disc_opts = MQTTAsync_disconnectOptions_initializer;
    int rc;
    int ch;

    std::string host = config["host"];
    std::string port = to_string(config["port"]);
    std::string endpoint = host + ":" + port;
    std::string sub_topic = config["sub_topic"];
    std::cout << "endpoint : " << endpoint.c_str() << std::endl;
    int QOS = config["QOS"];
    
    if ((rc = MQTTAsync_create(&client, host.c_str(), CLIENTID, 
                    MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTASYNC_SUCCESS) {
            printf("Failed to create client, return code %d\n", rc);
            goto exit;
        }
 
    if ((rc = MQTTAsync_setCallbacks(client, client, connlost, 
                    msgarrvd, NULL)) != MQTTASYNC_SUCCESS) {
            printf("Failed to set callbacks, return code %d\n", rc);
            goto destroy_exit;
        }

    conn_opts.cleansession = 1;
    conn_opts.onSuccess = onConnect;
    conn_opts.onFailure = onConnectFailure;
    conn_opts.context = client;
    
    if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS)
        {
            printf("Failed to start connect, return code %d\n", rc);
            goto destroy_exit;
        }

    if (finished){
            goto exit; }
    
    for(;;){}

    disc_opts.onSuccess = onDisconnect;
    disc_opts.onFailure = onDisconnectFailure;
    if ((rc = MQTTAsync_disconnect(client, &disc_opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start disconnect, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto destroy_exit;
    }
destroy_exit:
        MQTTAsync_destroy(&client);
exit:
        return;

        }
        
    } // namespace mqtt
} // namespace karlo
