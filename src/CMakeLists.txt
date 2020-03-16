

add_subdirectory(sm)
add_subdirectory(cstrike)
add_subdirectory(gameplay)
add_subdirectory(util)

add_library(z4d SHARED 
        ${CMAKE_CURRENT_SOURCE_DIR}/extension.cpp
        ${CMAKE_CURRENT_LIST_DIR}/event.cpp
        ${CMAKE_CURRENT_LIST_DIR}/event.h
        ${CMAKE_CURRENT_LIST_DIR}/client.cpp
        ${CMAKE_CURRENT_LIST_DIR}/client.h
        ${CMAKE_CURRENT_LIST_DIR}/serverdll.cpp
        ${CMAKE_CURRENT_LIST_DIR}/serverdll.h
        ${CMAKE_CURRENT_LIST_DIR}/command.cpp
        ${CMAKE_CURRENT_LIST_DIR}/command.h
        ${CMAKE_CURRENT_LIST_DIR}/entity_listener.cpp
        ${CMAKE_CURRENT_LIST_DIR}/entity_listener.h
)

target_compile_definitions(z4d PRIVATE
        -DSE_EPISODEONE=1 -DSE_DARKMESSIAH=2 -DSE_ORANGEBOX=3 -DSE_BLOODYGOODTIME=4 -DSE_EYE=5 -DSE_CSS=6 -DSE_ORANGEBOXVALVE=7 -DSE_LEFT4DEAD=8 -DSE_LEFT4DEAD2=9 -DSE_ALIENSWARM=10 -DSE_PORTAL2=11 -DSE_CSGO=12
        )

target_compile_definitions(z4d PRIVATE
        -DSDK_EXPORTS -DSOURCEMOD_BUILD -DSOURCE_ENGINE=12)

target_include_directories(z4d PUBLIC
        ./
        ${Protobuf_INCLUDE_DIRS}
        ${CMAKE_CURRENT_BINARY_DIR} # for protobuf
        )

target_link_libraries(z4d PUBLIC smsdk_ext z4d_sm z4d_util z4d_cstrike z4d_gameplay hlsdk_proto json hydb)
set_target_properties(z4d PROPERTIES PREFIX "" SUFFIX ".csgo.ext${HL2SDK_LIB_SHARED_EXT}")
