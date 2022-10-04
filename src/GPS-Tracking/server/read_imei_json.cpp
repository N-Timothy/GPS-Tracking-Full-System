#include "GPS-Tracking/server/read_imei_json.hpp"

namespace karlo {
  namespace server {

    std::string getUsername() {
      uid_t userid;
      struct passwd* pwd;
      userid = getuid();
      pwd = getpwuid(userid);
      return pwd->pw_name;
    }

    std::vector<json> readImeiJson(std::string jsonFilename) {
      std::ifstream ifs(jsonFilename);
      json jf = json::parse(ifs);

      std::vector<json> imei_list;
      for (unsigned i = 0; i < jf["gps_id"].size(); i++) {
        imei_list.push_back(jf["gps_id"][i]["imei"]);
      }
      return imei_list;
    }

  } // namespace server
} // namespace karlo
