#ifndef GPS_TRACKING_FULL_SYSTEM_MAIN_READ_IMEI_JSON_HPP
#define GPS_TRACKING_FULL_SYSTEM_MAIN_READ_IMEI_JSON_HPP

#define IMEI_JSON_FILENAME ".config/tracker/gps_imei.json"

#include <unistd.h>
#include <pwd.h>

#include <nlohmann/json.hpp>
#include <fstream>
#include <vector>

namespace karlo {
  namespace server {

    using json = nlohmann::json;

    std::vector<json> readImeiJson(std::string jsonFilename);
  } // namespace server
} // namespace karlo

#endif //GPS_TRACKING_FULL_SYSTEM_MAIN_READ_IMEI_JSON_HPP
