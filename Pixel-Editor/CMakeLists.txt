#设置最小版本
cmake_minimum_required(VERSION 3.5)
project(Pixel-Editor)


file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets
DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB PixelEditorSrc
"src/*.h"
"src/*.cpp")

add_executable(Pixel-Editor ${PixelEditorSrc})

target_link_libraries(Pixel-Editor PRIVATE Pixel)

target_include_directories(Pixel-Editor
PRIVATE
"../Pixel/vendor/spdlog/include"
"../Pixel/src"
"../Pixel/vendor"
"../Pixel/vendor/glm"
)

target_compile_definitions(Pixel-Editor PRIVATE
PX_PLATFORM_WINDOWS
PX_DEBUG
_CRT_SECURE_NO_WARNINGS
)

target_compile_features(Pixel-Editor
PRIVATE
cxx_std_17
)



