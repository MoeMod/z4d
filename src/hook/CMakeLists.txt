

add_library(z4d_hook STATIC hook_mp.cpp)

target_include_directories(z4d_hook PRIVATE
        ${HL2SDK_PATH}/public
        ${HL2SDK_PATH}/game
        )

target_link_libraries(z4d_hook PUBLIC mathlib)