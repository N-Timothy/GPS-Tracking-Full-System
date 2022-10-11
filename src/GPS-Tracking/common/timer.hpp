#ifndef _GPS_TRACKING_COMMON_TIMER_HPP_
#define _GPS_TRACKING_COMMON_TIMER_HPP_

#include <map>
#include <ctime>

namespace karlo {
    namespace common {

        extern std::map<int, std::pair<time_t, bool>> TIMEOUT;

        void timer();

        void delete_timeout(int id);

        void add_timeout(int id);
    
    } // namespace common
} // namespace common

#endif // _GPS_TRACKING_COMMON_TIMER_HPP_
