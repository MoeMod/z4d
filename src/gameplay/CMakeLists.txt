file(RELATIVE_PATH TARGE_NAME "${CMAKE_CURRENT_LIST_DIR}/../" "${CMAKE_CURRENT_LIST_DIR}")
set(TARGET_NAME ${CMAKE_PROJECT_NAME}_${TARGE_NAME})

add_library(z4d_gameplay INTERFACE)

target_sources(z4d_gameplay INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/qqlogin.cpp
	${CMAKE_CURRENT_LIST_DIR}/qqlogin.h
	${CMAKE_CURRENT_LIST_DIR}/itemown.cpp
	${CMAKE_CURRENT_LIST_DIR}/itemown.h
	${CMAKE_CURRENT_LIST_DIR}/itemshop.cpp
	${CMAKE_CURRENT_LIST_DIR}/itemshop.h
	${CMAKE_CURRENT_LIST_DIR}/gameplay.cpp
	${CMAKE_CURRENT_LIST_DIR}/gameplay.h
	${CMAKE_CURRENT_LIST_DIR}/votekick.cpp
	${CMAKE_CURRENT_LIST_DIR}/votekick.h
	${CMAKE_CURRENT_LIST_DIR}/votemap.cpp
	${CMAKE_CURRENT_LIST_DIR}/votemap.h
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
	${CMAKE_CURRENT_LIST_DIR}/teammgr.cpp
	${CMAKE_CURRENT_LIST_DIR}/teammgr.h
	${CMAKE_CURRENT_LIST_DIR}/mapmgr.cpp
	${CMAKE_CURRENT_LIST_DIR}/mapmgr.h
	${CMAKE_CURRENT_LIST_DIR}/admin.cpp
	${CMAKE_CURRENT_LIST_DIR}/admin.h
	${CMAKE_CURRENT_LIST_DIR}/chat.cpp
	${CMAKE_CURRENT_LIST_DIR}/chat.h
	${CMAKE_CURRENT_LIST_DIR}/zombie.cpp
	${CMAKE_CURRENT_LIST_DIR}/zombie.h
	${CMAKE_CURRENT_LIST_DIR}/zmarket.cpp
	${CMAKE_CURRENT_LIST_DIR}/zmarket.h
	${CMAKE_CURRENT_LIST_DIR}/item_grenadepack.cpp
	${CMAKE_CURRENT_LIST_DIR}/item_grenadepack.h
	${CMAKE_CURRENT_LIST_DIR}/death.cpp
	${CMAKE_CURRENT_LIST_DIR}/death.h
	${CMAKE_CURRENT_LIST_DIR}/mod/mod.cpp
	${CMAKE_CURRENT_LIST_DIR}/mod/mod.h
	${CMAKE_CURRENT_LIST_DIR}/mod/mod_zp.cpp
	${CMAKE_CURRENT_LIST_DIR}/mod/mod_zp.h
)
target_include_directories(z4d_gameplay INTERFACE ${CMAKE_CURRENT_LIST_DIR})