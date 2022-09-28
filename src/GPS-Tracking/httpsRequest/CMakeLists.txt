add_library(GPS-Tracking-httpsRequest
            httpsRequest.cpp
            post.cpp
            httplib.h)

target_link_libraries(GPS-Tracking-httpsRequest PRIVATE
                      crypto
                      ssl)

target_include_directories(GPS-Tracking-httpsRequest PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
    "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/src>")

set_target_properties(
    GPS-Tracking-httpsRequest
    PROPERTIES CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

target_set_warnings(GPS-Tracking-httpsRequest ENABLE ALL DISABLE Annoying)
