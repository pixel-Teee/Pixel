#设置最小版本
cmake_minimum_required(VERSION 3.5)
project(Pixel)

file(GLOB PixelSrc
"src/*.h"
"src/*.cpp"
"src/Pixel/Core/*.h"
"src/Pixel/Events/*.h"
"src/Pixel/ImGui/*.h"
"src/Pixel/Renderer/*.h"
"src/Pixel/Debug/*.h"
"src/Pixel/Scene/*.h"
"src/Pixel/Core/*.cpp"
"src/Pixel/Events/*.cpp"
"src/Pixel/ImGui/*.cpp"
"src/Pixel/Renderer/*.cpp"
"src/Pixel/Debug/*.cpp"
"src/Pixel/Scene/*.cpp"
"src/Platform/OpenGL/*.h"
"src/Platform/OpenGL/*.cpp"
"src/Platform/Windows/*.h"
"src/Platform/Windows/*.cpp"
"vendor/stb_image/*.h"
"vendor/stb_image/*.cpp"
"vendor/glm/glm/*.hpp"
"vendor/glm/glm/*.inl"
"vendor/entt/include/*.hpp"
"vendor/yaml-cpp/*.h"
)

source_group(TREE ${CMAKE_SOURCE_DIR}/Pixel FILES ${PixelSrc})
#message(${CMAKE_SOURCE_DIR})
#message(${PixelSrc})
add_library(Pixel STATIC ${PixelSrc})

#链接了Pixel的模块，不会包含这些文件
target_include_directories(Pixel PRIVATE
"src"
"vendor/spdlog/include"
"vendor/GLFW/include"
"vendor/Glad/include"
"vendor/imgui"
"vendor/glm"
"vendor/stb_image"
"vendor/entt/include"
"vendor/yaml-cpp/include"
)

target_compile_definitions(Pixel
    PRIVATE PX_PLATFORM_WINDOWS
    PRIVATE PX_BUILD_DLL
    PRIVATE GLFW_INCLUDE_NONE
    PRIVATE IMGUI_IMPL_OPENGL_LOADER_CUSTOM
    PRIVATE PX_DEBUG
    PRIVATE YAML_CPP_STATIC_DEFINE
)

#预编译头文件
target_precompile_headers(Pixel
PUBLIC "src/pxpch.h"
)

find_package(OpenGL REQUIRED)

if(OPENGL_FOUND)
    message("Found OpenGL")
else()
    message("Not Found OpenGL")
endif()

target_link_libraries(Pixel PUBLIC glfw
PUBLIC GLAD
PUBLIC ImGui
PUBLIC ${OPENGL_LIBRARIES}
PRIVATE yaml-cpp
)



