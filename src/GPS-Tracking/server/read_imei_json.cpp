#include "GPS-Tracking/server/read_imei_json.hpp"
#include "GPS-Tracking/database/database.hpp"

#include <fstream>

namespace karlo {
  namespace server {

    std::vector<json> readImeiJson(std::string jsonFilename=JSON_FILENAME) {
      std::ifstream ifs(jsonFilename);
      json jf = json::parse(ifs);

      std::vector<json> imei_list;
      for (int i = 0; i < jf["gps_id"].size(); i++) {
        imei_list.push_back(jf["gps_id"][i]["imei"]);
      }
      return imei_list
    }

  } // namespace server
} // namespace karlo
