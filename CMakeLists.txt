cmake_minimum_required(VERSION 3.12)
project(z4d)

include(smsdk_ext-cmake/smsdk_ext.cmake)

# JSON
#find_package(nlohmann_json 3.2.0 REQUIRED)
#add_target(json ALIAS nlohmann_json::nlohmann_json)
add_library(json INTERFACE)
target_include_directories(json INTERFACE "${CMAKE_SOURCE_DIR}/extern/json")

add_subdirectory(smcppsdk)
add_subdirectory(src)