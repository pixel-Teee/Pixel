#������С�汾
cmake_minimum_required(VERSION 3.5)
project(SandBox)

file(GLOB SandBoxSrc
"src/*.h"
"src/*.cpp")

add_executable(SandBox ${SandBoxSrc})

target_link_libraries(SandBox PRIVATE Pixel)

target_compile_definitions(SandBox PRIVATE
PX_PLATFORM_WINDOWS
PX_DEBUG
)

target_compile_features(SandBox
PRIVATE
cxx_std_17
)



