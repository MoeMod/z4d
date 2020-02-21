cmake_minimum_required(VERSION 3.12)
project(z4d)

set(CMAKE_CXX_STANDARD 14)

set(HL2SDK_PATH "${CMAKE_SOURCE_DIR}/hl2sdk-csgo")
set(SOURCEMOD_PATH "${CMAKE_SOURCE_DIR}/sourcemod")

include(smsdk_ext.cmake)

add_library(z4d SHARED
        extension.cpp
        )

target_compile_definitions(z4d PRIVATE
        -DSE_EPISODEONE=1 -DSE_DARKMESSIAH=2 -DSE_ORANGEBOX=3 -DSE_BLOODYGOODTIME=4 -DSE_EYE=5 -DSE_CSS=6 -DSE_ORANGEBOXVALVE=7 -DSE_LEFT4DEAD=8 -DSE_LEFT4DEAD2=9 -DSE_ALIENSWARM=10 -DSE_PORTAL2=11 -DSE_CSGO=12
        )

target_compile_definitions(z4d PRIVATE
        -DSDK_EXPORTS -DSOURCEMOD_BUILD -DSOURCE_ENGINE=12)

target_include_directories(z4d PUBLIC
        ./
)

target_link_libraries(z4d PUBLIC smsdk_ext)
set_target_properties(z4d PROPERTIES PREFIX "" SUFFIX ".ext${HL2SDK_LIB_SHARED_EXT}")
