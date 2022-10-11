add_library(GPS-Tracking-common
            timer.cpp )


target_link_libraries(GPS-Tracking-common PRIVATE)

target_include_directories(GPS-Tracking-common PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
    "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/src>")

set_target_properties(
    GPS-Tracking-common
    PROPERTIES CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

target_set_warnings(GPS-Tracking-common ENABLE ALL DISABLE Annoying)
