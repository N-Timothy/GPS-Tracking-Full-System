#include "GPS-Tracking/core/config.hpp"

#include <sys/types.h>
#include <pwd.h>
#include <string>
#include <iostream>
#include <fstream>

#include "GPS-Tracking/httpsRequest/httpsRequest.hpp"
#include "GPS-Tracking/database/database.hpp"
#include "GPS-Tracking/mqtt/connection.hpp"
#include "GPS-Tracking/server/tcp.hpp"

namespace karlo {
    namespace core {
        namespace config {

            json data;

            std::string getUsername() {

                uid_t userid;
                struct passwd* pwd;
                userid = getuid();
                pwd = getpwuid(userid);
                return pwd->pw_name;
            }

            void config () {
            
                std::string config = "/home/" + getUsername() + "/" + CONFIG_DIR;
                
                std::cout << "CONFIG: " << config << std::endl;

                std::ifstream ifs(config);
                if(ifs){
                    data = json::parse(ifs);
                } 

                httpsRequest::setHttpsConfig(data["https_request"]);
                
                database::setDatabaseConfig(data["database"]);

                mqtt::setMqttConfig(data["mqtt"]);

                server::setTcpConfig(data["tcp"]);

                ifs.close();


            }

        } // namespace config
    } // namespace core
} // namespace karlo
