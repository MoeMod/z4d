

add_library(z4d_hook INTERFACE)

target_sources(z4d_hook INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/hook_mp.cpp
    ${CMAKE_CURRENT_LIST_DIR}/hook_mp.h
    ${CMAKE_CURRENT_LIST_DIR}/hook_cs.cpp
    ${CMAKE_CURRENT_LIST_DIR}/hook_cs.h
    ${CMAKE_CURRENT_LIST_DIR}/hook_types.h
    ${CMAKE_CURRENT_LIST_DIR}/hook_takedamageinfo.h
    ${CMAKE_CURRENT_LIST_DIR}/hook_result.h
)

target_include_directories(z4d_hook INTERFACE
        ${HL2SDK_PATH}/public
        ${HL2SDK_PATH}/game
        )

target_link_libraries(z4d_hook INTERFACE mathlib)