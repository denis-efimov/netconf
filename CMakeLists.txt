cmake_minimum_required(VERSION 2.8)

project(netconf)

aux_source_directory(./ SOURCE_EXE)

add_executable(netconf ${SOURCE_EXE})

set_target_properties(netconf PROPERTIES COMPILE_FLAGS "`pkg-config --cflags dbus-1`")

SET(CMAKE_CXX_FLAGS "-std=c++11")

#set( CMAKE_BUILD_TYPE Debug )

target_link_libraries(netconf fcgi++ fcgi pthread dbus-1)
