file(RELATIVE_PATH TARGE_NAME "${CMAKE_CURRENT_LIST_DIR}/../" "${CMAKE_CURRENT_LIST_DIR}")
set(TARGET_NAME ${CMAKE_PROJECT_NAME}_${TARGE_NAME})

add_library(z4d_gameplay INTERFACE)

target_sources(z4d_gameplay INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/zmarket.cpp
	${CMAKE_CURRENT_LIST_DIR}/zmarket.h
)

target_include_directories(z4d_gameplay INTERFACE
	${CMAKE_CURRENT_LIST_DIR}
)
