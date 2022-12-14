#include "GPS-Tracking/database/database.hpp"
#include "GPS-Tracking/database/create.hpp"
#include "GPS-Tracking/database/read.hpp"
#include "GPS-Tracking/core/config.hpp"

#include <iostream>
#include <condition_variable>

#include <pwd.h>
#include <fstream>
#include <sys/types.h>
#include <string>
#include <iostream>
#include <unistd.h>


bool ready = true;

namespace karlo {
  namespace database {

    std::condition_variable cv;

    json config;

    // this is a temporary solution
    std::string getURI() {

        json data;
        uid_t userid;
        struct passwd* pwd;
        userid = getuid();
        pwd = getpwuid(userid);
                
        std::string config = "/home/" + (std::string)pwd->pw_name + "/.config/tracker/config.json";
                
        std::ifstream ifs(config);
        if(ifs){
            data = json::parse(ifs);
        } 
        return data["database"]["uri"];
    }

    std::string URI = getURI();

    // Connecting to database
    mongocxx::instance instance{};
    mongocxx::uri uri(URI);
    mongocxx::client client(uri);
    mongocxx::database db;    
    mongocxx::collection collection;

    void setDatabaseConfig(json databaseConfig){
        config = databaseConfig;
        db = client[(std::string) config["table"]];
        collection = db[(std::string) config["collection"]];
    }


    void createData(trackingData data) {

      ready = false;

      create(data, collection);

      ready = true;
      cv.notify_one();
    }

    json readData(std::string imei) {

      ready = false;
      
      json res = readOne(collection, imei);

      ready = true;
      cv.notify_one();

      return res;
    }

    std::vector<json> readData() {

      ready = false;

      std::vector<json> res = readAll(collection);

      ready = true;
      cv.notify_one();

      return res;
    }

  } // namespace database
} // namesapce karlo
