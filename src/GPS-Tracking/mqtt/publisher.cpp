#include "GPS-Tracking/mqtt/publisher.hpp"
#include "GPS-Tracking/database/database.hpp"

#define CLIENTID "GPS-Tracker-Pub"

#include <iostream>
#include <string>
#include <mutex>
#include <condition_variable>
#include <unistd.h>
#include <chrono>
#include <thread>

namespace karlo { 
    namespace mqtt {

        std::mutex m;
        std::condition_variable cv;

        using namespace std::literals::chrono_literals;

        using json = nlohmann::json;
        
int finishedPub = 0;
std::string imei;
 
void connlostPub(void *context, char *cause)
{
        MQTTAsync client = (MQTTAsync)context;
        MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
        int rc;
 
        printf("\nConnection lost\n");
        printf("     cause: %s\n", cause);
 
        printf("Reconnecting\n");
        conn_opts.keepAliveInterval = 20;
        conn_opts.cleansession = 1;
        if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS)
        {
                printf("Failed to start connect, return code %d\n", rc);
                finishedPub = 1;
        }
}
 
void onDisconnectFailurePub(void* context, MQTTAsync_failureData* response)
{
        printf("Disconnect failed\n");
        finishedPub = 1;
}
 
void onDisconnectPub(void* context, MQTTAsync_successData* response)
{
        printf("Successful disconnection\n");
        finishedPub = 1;
}
 
void onSendFailure(void* context, MQTTAsync_failureData* response)
{
        MQTTAsync client = (MQTTAsync)context;
        MQTTAsync_disconnectOptions opts = MQTTAsync_disconnectOptions_initializer;
        int rc;
 
        printf("Message send failed token %d error code %d\n", response->token, response->code);
        opts.onSuccess = onDisconnectPub;
        opts.onFailure = onDisconnectFailurePub;
        opts.context = client;
        if ((rc = MQTTAsync_disconnect(client, &opts)) != MQTTASYNC_SUCCESS)
        {
                printf("Failed to start disconnect, return code %d\n", rc);
                exit(EXIT_FAILURE);
        }
}
 
void onSend(void* context, MQTTAsync_successData* response)
{
        MQTTAsync client = (MQTTAsync)context;
        MQTTAsync_disconnectOptions opts = MQTTAsync_disconnectOptions_initializer;
        int rc;
 
        printf("Message with token value %d delivery confirmed\n", response->token);
        opts.onSuccess = onDisconnectPub;
        opts.onFailure = onDisconnectFailurePub;
        opts.context = client;
        if ((rc = MQTTAsync_disconnect(client, &opts)) != MQTTASYNC_SUCCESS)
        {
                printf("Failed to start disconnect, return code %d\n", rc);
                exit(EXIT_FAILURE);
        }
}
 
 
void onConnectFailurePub(void* context, MQTTAsync_failureData* response)
{
        printf("Connect failed, rc %d\n", response ? response->code : 0);
        finishedPub = 1;
}

int messageArrived(void* context, char* topicName, int topicLen, MQTTAsync_message* m)
{
        // not expecting any messages
        return 1;
}
 
 
    void onConnectPub(void* context, MQTTAsync_successData* response)
    {

        MQTTAsync client = (MQTTAsync)context;
        MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
        MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
        int rc;

        std::string pub_topic = config["pub_topic"];

	    std::cout << "We are now connected to the broker! " << std::endl;

        std::unique_lock<std::mutex> lk(m);
        if(!cv.wait_until(lk, std::chrono::system_clock::now() + 3s, []{return ready;})){
            std::cout << "timeout" << std::endl;
            return;
        } else {

            json data = database::readData(imei);
            if (data.is_null()){
                return;
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

 
            opts.onSuccess = onSend;
            opts.onFailure = onSendFailure;
            opts.context = client;
            pubmsg.payload = (void*) PAYLOAD.dump().c_str();
            pubmsg.payloadlen = strlen(PAYLOAD.dump().c_str());
            pubmsg.qos = config["QOS"];
            pubmsg.retained = 0;
            if ((rc = MQTTAsync_sendMessage(client, pub_topic.c_str(), &pubmsg, &opts)) != MQTTASYNC_SUCCESS)
            {
                printf("Failed to start sendMessage, return code %d\n", rc);
                return;
            }
        }
    }

        int publisher(std::string _imei) {

            MQTTAsync client;
            MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
            int rc;
            imei = _imei;

            std::string host = config["host"];
            std::string port = to_string(config["port"]);
            std::string endpoint = host + ":" + port;
            std::cout << "endpoint : " << endpoint.c_str() << std::endl;
            
            if ((rc = MQTTAsync_create(&client, host.c_str(), CLIENTID, 
                            MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTASYNC_SUCCESS)
            {
                printf("Failed to create client object, return code %d\n", rc);
                return -2;
            }
 
            if ((rc = MQTTAsync_setCallbacks(client, NULL, connlostPub, 
                            messageArrived, NULL)) != MQTTASYNC_SUCCESS)
            {
                printf("Failed to set callback, return code %d\n", rc);
                return -2;
            }

            conn_opts.cleansession = 1;
            conn_opts.onSuccess = onConnectPub;
            conn_opts.onFailure = onConnectFailurePub;
            conn_opts.context = client;
            if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS)
            {
                printf("Failed to start connect, return code %d\n", rc);
            }
            
            while (!finishedPub){
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            } 
 
            MQTTAsync_destroy(&client);

            return 0;
        }

    } // namespace mqtt

} // namespace karlo
