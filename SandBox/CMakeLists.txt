#设置最小版本
cmake_minimum_required(VERSION 3.5)
project(SandBox)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets
DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB SandBoxSrc
"src/*.h"
"src/*.cpp")

add_executable(SandBox ${SandBoxSrc})

target_link_libraries(SandBox PRIVATE Pixel)

target_include_directories(SandBox
PRIVATE
"../Pixel/vendor/spdlog/include"
"../Pixel/src"
"../Pixel/vendor"
"../Pixel/vendor/glm"
)

target_compile_definitions(SandBox PRIVATE
PX_PLATFORM_WINDOWS
PX_DEBUG
)

target_compile_features(SandBox
PRIVATE
cxx_std_17
)



