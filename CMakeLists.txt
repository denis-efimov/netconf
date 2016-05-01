cmake_minimum_required(VERSION 2.8)

project(netconf)

aux_source_directory(./ SOURCE_EXE)

add_executable(netconf ${SOURCE_EXE})

SET(CMAKE_CXX_FLAGS "-std=c++11")

target_link_libraries(netconf fcgi++ fcgi)
