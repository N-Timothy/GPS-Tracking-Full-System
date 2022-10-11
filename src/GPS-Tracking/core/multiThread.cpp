#include "GPS-Tracking/core/multiThread.hpp"
#include "GPS-Tracking/core/config.hpp"

#include "GPS-Tracking/server/tcp.hpp"
#include "GPS-Tracking/httpsRequest/httpsRequest.hpp"
#include "GPS-Tracking/mqtt/subscriber.hpp"
#include "GPS-Tracking/common/timer.hpp"

#include <thread>

namespace karlo {
    namespace core {

        void multiThread() {

            config::config();

            std::thread timerThread(common::timer);
            std::thread httpsRequestThread(httpsRequest::connect);
            //std::thread mqttSubscriberThread(mqtt::subscriber);
            //std::thread tcpServerThread(server::tcpServer);

            timerThread.detach();
            httpsRequestThread.join();
           // mqttSubscriberThread.detach();
            //tcpServerThread.join();

        }

    } // namespace core
} // namespace karlo
