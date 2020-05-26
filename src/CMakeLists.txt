

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

target_include_directories(z4d PUBLIC
        ./
        )

target_link_libraries(z4d PUBLIC smsdk_ext hl2sdk_protobuf_csgo z4d_sm z4d_util z4d_cstrike z4d_gameplay json hydb)
set_target_properties(z4d PROPERTIES PREFIX "" SUFFIX ".csgo.ext${HL2SDK_LIB_SHARED_EXT}")
