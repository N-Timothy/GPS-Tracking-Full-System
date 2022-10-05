add_library(GPS-Tracking-core
            multiThread.cpp)

target_link_libraries(GPS-Tracking-core PRIVATE
                      GPS-Tracking-server
                      GPS-Tracking-httpsRequest
                      GPS-Tracking-mqtt
                      pthread)

target_include_directories(GPS-Tracking-core PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
    "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/src>")

set_target_properties(
    GPS-Tracking-core
    PROPERTIES CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

target_set_warnings(GPS-Tracking-core ENABLE ALL DISABLE Annoying)
