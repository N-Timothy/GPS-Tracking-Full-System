add_library(GPS-Tracking-mqtt
            connection.cpp
            publisher.cpp
            toggleController.cpp
            subscriber.cpp)

target_link_libraries(GPS-Tracking-mqtt PRIVATE
                      GPS-Tracking-database
                      mosquitto
                      paho-mqtt3c
                      pthread)

target_include_directories(GPS-Tracking-mqtt PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
    "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/src>")

set_target_properties(
    GPS-Tracking-mqtt
    PROPERTIES CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

target_set_warnings(GPS-Tracking-mqtt ENABLE ALL DISABLE Annoying)
