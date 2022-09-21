add_library(GPS-Tracking-database
            database.cpp
            create.cpp
            read.cpp)

target_link_libraries(GPS-Tracking-database PRIVATE
                      mongocxx
                      bsoncxx)

target_include_directories(GPS-Tracking-database PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
    "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/src>")

set_target_properties(
    GPS-Tracking-database
    PROPERTIES CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

target_set_warnings(GPS-Tracking-database ENABLE ALL DISABLE Annoying)
