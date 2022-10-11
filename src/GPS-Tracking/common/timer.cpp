#include "GPS-Tracking/common/timer.hpp"

#include <chrono>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <iomanip>

#include <iostream>

namespace karlo {
    namespace common {

        std::mutex m;
        std::condition_variable cv;

        std::map<int, std::pair<std::time_t, bool>> TIMEOUT;

        std::time_t time;

        bool timeoutMapAccess = true;

        void timer () {
            for(;;){
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                

                std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

            time = std::chrono::system_clock::to_time_t(now);

            std::unique_lock<std::mutex> lk(m);
            cv.wait(lk, []{return timeoutMapAccess;});

            timeoutMapAccess = false;
            
            for(std::map<int, std::pair<std::time_t, bool>>::iterator it = TIMEOUT.begin(); it != TIMEOUT.end(); ++it){


                std::cout <<"id  : " << it->first << " | timeout time : " << std::put_time(std::localtime(&it->second.first), "%T") << " | time : " << std::put_time(std::localtime(&time), "%T") << " | diff : " << std::difftime(time, it->second.first) << std::endl;


               if(std::difftime(time, it->second.first) > 0){
                    std::cout << "---------()---------" << std::endl;
                    it->second.second = true;
                    cv.notify_all();
               }
            }

            timeoutMapAccess = true;
            cv.notify_one();

            } // for (;;)
        }

        void delete_timeout(int id){

            std::unique_lock<std::mutex> lk(m);
            cv.wait(lk, []{return timeoutMapAccess;});

            timeoutMapAccess = false;

            TIMEOUT.erase(id);

            timeoutMapAccess = true;
            cv.notify_one();
        }

        void add_timeout(int id){
            TIMEOUT.insert(std::make_pair(id, std::make_pair(time, false)));
        }

    } // namespace karlo
} // namespace karlo
