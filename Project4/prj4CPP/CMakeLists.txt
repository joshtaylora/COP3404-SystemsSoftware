cmake_minimum_required(VERSION 2.8.11)
project(prj4CPP)

#recurse through subdirectories
add_subdirectory(Libraries)

add_executable(prj4CPP prj4.cpp)

target_link_libraries (assembler LINK_PUBLIC Libraries)