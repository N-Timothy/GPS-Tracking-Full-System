#ifndef GPS_TRACKING_FULL_SYSTEM_MAIN_READ_IMEI_JSON_HPP
#define GPS_TRACKING_FULL_SYSTEM_MAIN_READ_IMEI_JSON_HPP

#define JSON_FILENAME "gps_imei.json"

namespace karlo {
  namespace server {

    using json = nlohmann::json;

    std::vector<json> readImeiJson(std::string jsonFilename);
  } // namespace server
} // namespace karlo

#endif //GPS_TRACKING_FULL_SYSTEM_MAIN_READ_IMEI_JSON_HPP
