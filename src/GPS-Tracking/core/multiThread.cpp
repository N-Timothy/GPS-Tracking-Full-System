#include "GPS-Tracking/core/multiThread.hpp"

#include "GPS-Tracking/server/tcp.hpp"
#include "GPS-Tracking/httpsRequest/httpsRequest.hpp"
#include "GPS-Tracking/mqtt/subscriber.hpp"

#include <thread>

namespace karlo {
    namespace core {

        void multiThread() {
            
            //server::tcpServer

            std::thread httpsRequestThread(httpsRequest::connect, 5000);
         //   std::thread mqttSubscriberThread(mqtt::subscriber);
            httpsRequestThread.detach();
            mqtt::subscriber();
        //    mqttSubscriberThread.detach();
        }

    } // namespace core
} // namespace karlo
