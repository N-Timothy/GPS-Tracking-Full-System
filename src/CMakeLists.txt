add_subdirectory(GPS-Tracking/database)
add_subdirectory(GPS-Tracking/server)

add_executable(${PROJECT_NAME}
                main.cpp
                ${META_FILES_TO_INCLUDE}
                ${RESOURCE_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE
    GPS-Tracking-database
    GPS-Tracking-server
    )

target_include_directories(${PROJECT_NAME} PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
    "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/src>")

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO)

target_set_warnings(${PROJECT_NAME} ENABLE ALL DISABLE Annoying)
