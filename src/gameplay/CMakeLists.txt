file(RELATIVE_PATH TARGE_NAME "${CMAKE_CURRENT_LIST_DIR}/../" "${CMAKE_CURRENT_LIST_DIR}")
set(TARGET_NAME ${CMAKE_PROJECT_NAME}_${TARGE_NAME})

add_library(z4d_gameplay INTERFACE)

target_sources(z4d_gameplay INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/gameplay.cpp
	${CMAKE_CURRENT_LIST_DIR}/gameplay.h
	${CMAKE_CURRENT_LIST_DIR}/rtv.cpp
	${CMAKE_CURRENT_LIST_DIR}/rtv.h
	${CMAKE_CURRENT_LIST_DIR}/say_menu.cpp
	${CMAKE_CURRENT_LIST_DIR}/say_menu.h
	${CMAKE_CURRENT_LIST_DIR}/iplocation.cpp
	${CMAKE_CURRENT_LIST_DIR}/iplocation.h
	${CMAKE_CURRENT_LIST_DIR}/random_reciter.cpp
	${CMAKE_CURRENT_LIST_DIR}/random_reciter.h
	${CMAKE_CURRENT_LIST_DIR}/alarm.cpp
	${CMAKE_CURRENT_LIST_DIR}/alarm.h
	${CMAKE_CURRENT_LIST_DIR}/mapmgr.cpp
	${CMAKE_CURRENT_LIST_DIR}/mapmgr.h
	${CMAKE_CURRENT_LIST_DIR}/admin.cpp
	${CMAKE_CURRENT_LIST_DIR}/admin.h
	${CMAKE_CURRENT_LIST_DIR}/chat.cpp
	${CMAKE_CURRENT_LIST_DIR}/chat.h
)
target_include_directories(z4d_gameplay INTERFACE ${CMAKE_CURRENT_LIST_DIR})