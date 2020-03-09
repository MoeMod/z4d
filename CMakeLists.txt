cmake_minimum_required(VERSION 3.12)
project(z4d)

set(CMAKE_CXX_STANDARD 17)
if(MSVC)
	add_compile_options(/wd4819 /permissive-)
elseif(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
	add_compile_options(-Wno-register)
endif()

set(HL2SDK_PATH "${CMAKE_SOURCE_DIR}/hl2sdk-csgo")
set(SOURCEMOD_PATH "${CMAKE_SOURCE_DIR}/sourcemod")
set(METAMOD_SOURCE_PATH "${CMAKE_SOURCE_DIR}/metamod-source")
include(cmake/smsdk_ext.cmake)

add_subdirectory(src)