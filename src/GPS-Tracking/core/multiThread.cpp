#include "GPS-Tracking/core/multiThread.hpp"
#include "GPS-Tracking/core/config.hpp"

#include "GPS-Tracking/server/tcp.hpp"
#include "GPS-Tracking/httpsRequest/httpsRequest.hpp"
#include "GPS-Tracking/mqtt/subscriber.hpp"

#include <thread>
#include <chrono>

namespace karlo {
    namespace core {

        void multiThread() {

            config::config();

  //          std::thread httpsRequestThread(httpsRequest::connect);
 //           std::thread mqttSubscriberThread(mqtt::subscriber);
            std::thread tcpServerThread(server::tcpServer);

            std::this_thread::sleep_for(std::chrono::milliseconds(250));
    //        httpsRequestThread.detach();

            std::this_thread::sleep_for(std::chrono::milliseconds(250));
//            mqttSubscriberThread.detach();

            std::this_thread::sleep_for(std::chrono::milliseconds(250));
            tcpServerThread.join();

        }

    } // namespace core
} // namespace karlo
