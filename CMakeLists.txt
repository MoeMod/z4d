cmake_minimum_required(VERSION 3.12)
project(z4d)

include(cmake/smsdk_ext.cmake)

# JSON
#find_package(nlohmann_json 3.2.0 REQUIRED)
#add_target(json ALIAS nlohmann_json::nlohmann_json)
add_library(json INTERFACE)
target_include_directories(json INTERFACE "${CMAKE_SOURCE_DIR}/extern/json")

include_directories(${CMAKE_SOURCE_DIR}/wrappers)

# OpenSSL
find_package(OpenSSL REQUIRED)

# Boost
# vcpkg install boost-asio
# vcpkg install boost-beast
# vcpkg install boost-smart-ptr
#add_subdirectory(${CMAKE_SOURCE_DIR}/extern/boost-cmake)
find_package(Boost REQUIRED date_time coroutine)
add_subdirectory(${CMAKE_SOURCE_DIR}/extern/mysql-asio)

add_subdirectory(hydb)
add_subdirectory(src)