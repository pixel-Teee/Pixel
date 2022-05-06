#设置最小版本
cmake_minimum_required(VERSION 3.5)
project(lua)

file(GLOB_RECURSE src
"./*.h"
"./*.c")

add_library(lua STATIC ${src})

target_include_directories(lua PRIVATE
"./*.h"
)