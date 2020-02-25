
add_library(z4d_cstrike INTERFACE)

target_sources(z4d_cstrike INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/csgo_const.h
)

target_include_directories(z4d_cstrike INTERFACE
	${CMAKE_CURRENT_LIST_DIR}
)
