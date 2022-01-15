#设置最小版本
cmake_minimum_required(VERSION 3.5)
project(PixelEditor)


file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets
DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Resources
DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/imgui.ini
DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB PixelEditorSrc
"src/*.h"
"src/*.cpp"
"src/Panels/*.h"
"src/Panels/*.cpp"
)

source_group(TREE ${CMAKE_SOURCE_DIR}/PixelEditor FILES ${PixelEditorSrc})

# message(${CMAKE_SOURCE_DIR}/PixelEditor/src)

add_executable(PixelEditor ${PixelEditorSrc})

target_link_libraries(PixelEditor PRIVATE Pixel)

target_include_directories(PixelEditor
PRIVATE
"../Pixel/vendor/spdlog/include"
"../Pixel/src"
"../Pixel/vendor"
"../Pixel/vendor/glm"
"../Pixel/vendor/entt/include"
"../Pixel/vendor/imguizmo"
"../Pixel/vendor/Box2D"
)

target_compile_definitions(PixelEditor PRIVATE
PX_PLATFORM_WINDOWS
PX_DEBUG
_CRT_SECURE_NO_WARNINGS
)

target_compile_features(PixelEditor
PRIVATE
cxx_std_17
)



