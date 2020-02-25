

add_subdirectory(hook)
add_subdirectory(cstrike)
add_subdirectory(util)

set(CMAKE_CXX_STANDARD_REQUIRED 14)

add_library(z4d SHARED 
     ${CMAKE_CURRENT_SOURCE_DIR}/extension.cpp
)

target_include_directories(z4d PRIVATE
        ${HL2SDK_PATH}/game
)

target_compile_definitions(z4d PRIVATE
        -DSE_EPISODEONE=1 -DSE_DARKMESSIAH=2 -DSE_ORANGEBOX=3 -DSE_BLOODYGOODTIME=4 -DSE_EYE=5 -DSE_CSS=6 -DSE_ORANGEBOXVALVE=7 -DSE_LEFT4DEAD=8 -DSE_LEFT4DEAD2=9 -DSE_ALIENSWARM=10 -DSE_PORTAL2=11 -DSE_CSGO=12
        )

target_compile_definitions(z4d PRIVATE
        -DSDK_EXPORTS -DSOURCEMOD_BUILD -DSOURCE_ENGINE=12)

target_include_directories(z4d PUBLIC
        ./
        )

target_link_libraries(z4d PUBLIC smsdk_ext z4d_hook z4d_util z4d_cstrike)
set_target_properties(z4d PROPERTIES PREFIX "" SUFFIX ".csgo.ext${HL2SDK_LIB_SHARED_EXT}")
