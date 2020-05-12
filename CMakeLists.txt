cmake_minimum_required(VERSION 3.12)
project(z4d)

set(CMAKE_CXX_STANDARD 17)
if(MSVC)
	add_compile_options(/wd4819 /wd4828 /wd5033 /permissive- /utf-8)
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS=1)
elseif(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
	add_compile_options(-Wno-register)
endif()

set(HL2SDK_PATH "${CMAKE_SOURCE_DIR}/hl2sdk-csgo" CACHE PATH "Path to hl2sdk-csgo")
set(SOURCEMOD_PATH "${CMAKE_SOURCE_DIR}/sourcemod" CACHE PATH "Path to sourcemod")
set(METAMOD_SOURCE_PATH "${CMAKE_SOURCE_DIR}/metamod-source" CACHE PATH "Path to sourcemm")
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