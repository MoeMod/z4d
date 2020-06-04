

add_library(z4d_sm INTERFACE)

target_sources(z4d_sm INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/sdkhooks.cpp
    ${CMAKE_CURRENT_LIST_DIR}/sdkhooks.h
    ${CMAKE_CURRENT_LIST_DIR}/sdktools.cpp
    ${CMAKE_CURRENT_LIST_DIR}/sdktools.h
    ${CMAKE_CURRENT_LIST_DIR}/cstrike.cpp
    ${CMAKE_CURRENT_LIST_DIR}/cstrike.h
    ${CMAKE_CURRENT_LIST_DIR}/sourcemod.cpp
    ${CMAKE_CURRENT_LIST_DIR}/sourcemod.h
    ${CMAKE_CURRENT_LIST_DIR}/sourcemod_hudtext.cpp
    ${CMAKE_CURRENT_LIST_DIR}/sourcemod_hudtext.h
    ${CMAKE_CURRENT_LIST_DIR}/sourcemod_players.cpp
    ${CMAKE_CURRENT_LIST_DIR}/sourcemod_players.h
    ${CMAKE_CURRENT_LIST_DIR}/hook_types.h
    ${CMAKE_CURRENT_LIST_DIR}/hook_takedamageinfo.h
    ${CMAKE_CURRENT_LIST_DIR}/hook_result.h
)

target_include_directories(z4d_sm INTERFACE
        ${HL2SDK_PATH}/public
        ${HL2SDK_PATH}/public/engine
        ${HL2SDK_PATH}/game
        ${HL2SDK_PATH}/public/game
        ${HL2SDK_PATH}/public/game/server
        ${HL2SDK_PATH}/public/game/shared
        )

target_link_libraries(z4d_sm INTERFACE mathlib CDetour)