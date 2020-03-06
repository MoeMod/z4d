cmake_minimum_required(VERSION 3.15)
project(z4d)

set(CMAKE_CXX_STANDARD 14)
if(MSVC)
	add_compile_options(/wd4819)
endif()

set(HL2SDK_PATH "${CMAKE_SOURCE_DIR}/hl2sdk-csgo")
set(SOURCEMOD_PATH "${CMAKE_SOURCE_DIR}/sourcemod")
set(METAMOD_SOURCE_PATH "${CMAKE_SOURCE_DIR}/metamod-source")
include(cmake/smsdk_ext.cmake)

# brew install protobuf
# vcpkg install protobuf
#find_package(Protobuf REQUIRED)
#include_directories(${Protobuf_INCLUDE_DIRS})
#include_directories(${CMAKE_CURRENT_BINARY_DIR})
# hack for msvc who can't find google/protobuf/*
#set(Protobuf_IMPORT_DIRS ${Protobuf_IMPORT_DIRS} ${Protobuf_INCLUDE_DIRS})
#protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS
#        "${HL2SDK_PATH}/public/engine/protobuf/netmessages.proto"
#        "${HL2SDK_PATH}/public/game/shared/csgo/protobuf/cstrike15_usermessages.proto"
#        )
set(PROTO_SRCS ${HL2SDK_PATH}/public/engine/protobuf/netmessages.pb.cc ${HL2SDK_PATH}/public/game/shared/csgo/protobuf/cstrike15_usermessages.pb.cc)
set(PROTO_HDRS ${HL2SDK_PATH}/public/engine/protobuf/netmessages.pb.h ${HL2SDK_PATH}/public/game/shared/csgo/protobuf/cstrike15_usermessages.pb.h)

add_library(hlsdk_protobuf STATIC 
	${HL2SDK_PATH}/common/protobuf-2.5.0/src/google/protobuf/stubs/atomicops_internals_x86_gcc.cc         
	${HL2SDK_PATH}/common/protobuf-2.5.0/src/google/protobuf/stubs/atomicops_internals_x86_msvc.cc        
	${HL2SDK_PATH}/common/protobuf-2.5.0/src/google/protobuf/stubs/common.cc                              
	${HL2SDK_PATH}/common/protobuf-2.5.0/src/google/protobuf/stubs/once.cc                                
	${HL2SDK_PATH}/common/protobuf-2.5.0/src/google/protobuf/stubs/stringprintf.cc                        
	${HL2SDK_PATH}/common/protobuf-2.5.0/src/google/protobuf/extension_set.cc                             
	${HL2SDK_PATH}/common/protobuf-2.5.0/src/google/protobuf/generated_message_util.cc                    
	${HL2SDK_PATH}/common/protobuf-2.5.0/src/google/protobuf/message_lite.cc                              
	${HL2SDK_PATH}/common/protobuf-2.5.0/src/google/protobuf/repeated_field.cc                            
	${HL2SDK_PATH}/common/protobuf-2.5.0/src/google/protobuf/wire_format_lite.cc                          
	${HL2SDK_PATH}/common/protobuf-2.5.0/src/google/protobuf/io/coded_stream.cc                           
	${HL2SDK_PATH}/common/protobuf-2.5.0/src/google/protobuf/io/zero_copy_stream.cc                       
	${HL2SDK_PATH}/common/protobuf-2.5.0/src/google/protobuf/io/zero_copy_stream_impl_lite.cc
	${HL2SDK_PATH}/common/protobuf-2.5.0/src/google/protobuf/stubs/strutil.cc                             
	${HL2SDK_PATH}/common/protobuf-2.5.0/src/google/protobuf/stubs/substitute.cc                          
	${HL2SDK_PATH}/common/protobuf-2.5.0/src/google/protobuf/stubs/structurally_valid.cc                  
	${HL2SDK_PATH}/common/protobuf-2.5.0/src/google/protobuf/descriptor.cc                                
	${HL2SDK_PATH}/common/protobuf-2.5.0/src/google/protobuf/descriptor.pb.cc                             
	${HL2SDK_PATH}/common/protobuf-2.5.0/src/google/protobuf/descriptor_database.cc                       
	${HL2SDK_PATH}/common/protobuf-2.5.0/src/google/protobuf/dynamic_message.cc                           
	${HL2SDK_PATH}/common/protobuf-2.5.0/src/google/protobuf/extension_set_heavy.cc                       
	${HL2SDK_PATH}/common/protobuf-2.5.0/src/google/protobuf/generated_message_reflection.cc              
	${HL2SDK_PATH}/common/protobuf-2.5.0/src/google/protobuf/message.cc                                   
	${HL2SDK_PATH}/common/protobuf-2.5.0/src/google/protobuf/reflection_ops.cc                            
	${HL2SDK_PATH}/common/protobuf-2.5.0/src/google/protobuf/service.cc                                   
	${HL2SDK_PATH}/common/protobuf-2.5.0/src/google/protobuf/text_format.cc                               
	${HL2SDK_PATH}/common/protobuf-2.5.0/src/google/protobuf/unknown_field_set.cc                         
	${HL2SDK_PATH}/common/protobuf-2.5.0/src/google/protobuf/wire_format.cc                               
	${HL2SDK_PATH}/common/protobuf-2.5.0/src/google/protobuf/io/gzip_stream.cc                            
	${HL2SDK_PATH}/common/protobuf-2.5.0/src/google/protobuf/io/printer.cc                                
	${HL2SDK_PATH}/common/protobuf-2.5.0/src/google/protobuf/io/tokenizer.cc                              
	${HL2SDK_PATH}/common/protobuf-2.5.0/src/google/protobuf/io/zero_copy_stream_impl.cc                  
	${HL2SDK_PATH}/common/protobuf-2.5.0/src/google/protobuf/compiler/importer.cc                         
	${HL2SDK_PATH}/common/protobuf-2.5.0/src/google/protobuf/compiler/parser.cc
)
target_include_directories(hlsdk_protobuf PUBLIC 
	${HL2SDK_PATH}/common/protobuf-2.5.0/src
	)
target_include_directories(hlsdk_protobuf PRIVATE 
	${CMAKE_SOURCE_DIR}/wrappers/protobuf
	)

add_library(hlsdk_proto STATIC ${PROTO_SRCS} ${PROTO_HDRS})
target_include_directories(hlsdk_proto PUBLIC 
	${HL2SDK_PATH}/public/engine/protobuf
	${HL2SDK_PATH}/public/game/shared/csgo/protobuf
	)
target_link_libraries(hlsdk_proto PUBLIC hlsdk_protobuf)

add_subdirectory(src)