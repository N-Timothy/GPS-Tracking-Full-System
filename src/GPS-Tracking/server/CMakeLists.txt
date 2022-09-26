add_library(GPS-Tracking-server
            tcp.cpp
            data.hpp
            server2.cpp)

target_link_libraries(GPS-Tracking-server PRIVATE
                      GPS-Tracking-database
                      pthread)

target_include_directories(GPS-Tracking-server PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
    "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/src>")

set_target_properties(
    GPS-Tracking-server
    PROPERTIES CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

target_set_warnings(GPS-Tracking-server ENABLE ALL DISABLE Annoying)
