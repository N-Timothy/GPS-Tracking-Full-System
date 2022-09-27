cmake_minimum_required(VERSION 3.16)

# add folder where are supportive functions
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Basic information about the project
project(GPS-Tracking
        VERSION 0.1 
        DESCRIPTION "GPS-Tracking with AWS server and mongoDB"
        LANGUAGES CXX)

option(
    ENABLE_WARNINGS_SETTINGS
    "Allow target_set_warnings to add flags and defines.
   Set this to OFF if you want to provide your own warning parameters."
    ON
)
option(ENABLE_LTO "Enable link time optimization" ON)

include(CPM)
include(LTO)
include(Warnings)

# Set additional project information
set(COMPANY "PT.Star Karlo Indonesia")
set(COMPANY_SHORT "Karlo")
set(COPYRIGHT "Copyright (c) 2022 Karlo. All rights reserved.")
set(IDENTIFIER "org.karlo.tracker")
#set(DOMAIN "karlo.id")
set(APP_NAME "GPS_Tracking_Server")
set(APP_NAME_SHORT "Tracking")

set(PROJECT_NAMESPACE "karlo")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#add_project_meta(META_FILES_TO_INCLUDE)

# -------------------------
# ENVIROMENTS
#---------------------------
find_package(OpenSSL REQUIRED)
if(OPENSSL_FOUND)
    set(CPPHTTPLIB_OPENSSL_SUPPORT TRUE)
endif()

## END OF PACKAGES -------------------------------------

add_subdirectory(src)
