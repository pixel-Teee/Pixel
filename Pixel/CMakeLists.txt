#设置最小版本
cmake_minimum_required(VERSION 3.5)
project(Pixel)

#append cmake module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

message("${CMAKE_SOURCE_DIR}/cmake")

add_subdirectory(${CMAKE_SOURCE_DIR}/MetaParser MetaParser)

#include the meta parser cmake script
include(MetaParser)

#------reflection module------
set(META_SOURCE_ROOT "${CMAKE_CURRENT_LIST_DIR}") # source root
set(META_GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/Generated") # generated directory

set(META_GENERATED_HEADERS "") # generated headers
set(META_GENERATED_SOURCES "") # generated sources

set(REFLECTION_HEADER "src/Pixel/Reflection/Reflection.h") # need parsed header, a set of headers
set(MODULE_HEADER "src/Pixel/Reflection/ReflectionModule.h") # module header
set(MODULE_TARGET "ReflectionModule") # module target

file(GLOB_RECURSE Headers
"src/Pixel/Scene/Components/*.h"
"src/Pixel/Core/UUID.h"
"src/Pixel/Scene/SceneCamera.h"
"src/Pixel/Renderer/3D/Material/Material.h"
"src/Pixel/Renderer/3D/Material/ShaderFunction.h"
"src/Pixel/Renderer/3D/Material/ShaderMainFunction.h"
"src/Pixel/Renderer/3D/Material/InputNode.h"
"src/Pixel/Renderer/3D/Material/OutputNode.h"
"src/Pixel/Renderer/3D/Material/PutNode.h"
"src/Pixel/Renderer/3D/Material/Mul.h"
"src/Pixel/Renderer/3D/Material/ConstFloatValue.h"
"src/Pixel/Renderer/3D/Material/ConstValue.h"
"src/Pixel/Animation/*.h"
)

set(HEADERS ${Headers}) 

set(REFLECTION_INCLUDES "")

# generate file name
meta_parser_prebuild(
    TARGET ${MODULE_TARGET} # module target
    GENERATED_DIR ${META_GENERATED_DIR} # generated directory
    SOURCE_ROOT ${META_SOURCE_ROOT} # source root
    HEADER_FILES ${HEADERS} # need parsed header
    MODULE_HEADER ${MODULE_HEADER} # module header
    OUT_MODULE_SOURCE META_MODULE_SOURCE # meta module source
    OUT_GENERATED_FILES META_GENERATED_FILES # meta module files
    OUT_INC META_GENERATED_HEADERS # out reference variable
    OUT_SRC META_GENERATED_SOURCES # out reference variable
)

# get a reflection module target
add_library(${MODULE_TARGET} ${META_GENERATED_FILES} ${META_MODULE_SOURCE} "src/Pixel/Reflection/ReflectionModule.h")

target_precompile_headers(${MODULE_TARGET} PRIVATE "${META_GENERATED_DIR}/pxpch.h")

target_include_directories(${MODULE_TARGET} PRIVATE "${META_GENERATED_DIR}" "src"
"vendor/spdlog/include"
"vendor/GLFW/include"
"vendor/Glad/include"
"vendor/imgui"
"vendor/glm"
"vendor/stb_image"
"vendor/entt/include"
"vendor/yaml-cpp/include"
"vendor/imguizmo"
"vendor/Box2D/include"
"vendor/assimp/include"
"vendor/imgui-node-editor"
"vendor/lua"
"vendor/bullet3/src"
"vendor/json/include"
"vendor/rttr/src")

target_link_libraries(${MODULE_TARGET} PUBLIC RTTR::Core_Lib)

target_compile_definitions(${MODULE_TARGET}
    PRIVATE PX_PLATFORM_WINDOWS
    PRIVATE PX_BUILD_DLL
    PRIVATE GLFW_INCLUDE_NONE
    PRIVATE IMGUI_IMPL_OPENGL_LOADER_CUSTOM
    PRIVATE PX_DEBUG
    PRIVATE YAML_CPP_STATIC_DEFINE
    PRIVATE PIXEL_3D
    PRIVATE __BT_DISABLE_SSE__
    PRIVATE B3_USE_CLEW
    PRIVATE GLM_FORCE_LEFT_HANDED
)

target_compile_features(${MODULE_TARGET}
PUBLIC cxx_std_17)

#------reflection module------

file(GLOB_RECURSE PixelSrc
"src/*.h"
"src/*.cpp"
"src/Pixel/Core/*.h"
"src/Pixel/Events/*.h"
"src/Pixel/ImGui/*.h"
"src/Pixel/Renderer/*.h"
"src/Pixel/Debug/*.h"
"src/Pixel/Scene/*.h"
"src/Pixel/Scene/Components/*.h"
"src/Pixel/Utils/*.h"
"src/Pixel/Math/*.h"
"src/Pixel/Physics/*.h"
"src/Pixel/Reflection/*.h"
"src/Pixel/Animation/*.h"
"src/Pixel/Core/*.cpp"
"src/Pixel/Events/*.cpp"
"src/Pixel/ImGui/*.cpp"
"src/Pixel/Renderer/*.cpp"
"src/Pixel/Renderer/3D/*.h"
"src/Pixel/Debug/*.cpp"
"src/Pixel/Scene/*.cpp"
"src/Pixel/Scene/Components/*.cpp"
"src/Pixel/Utils/*.cpp"
"src/Pixel/Math/*.cpp"
"src/Pixel/Physics/*.cpp"
"src/Pixel/Animation/*.cpp"
"src/Platform/OpenGL/*.h"
"src/Platform/OpenGL/*.cpp"
"src/Platform/DirectX/*.h"
"src/Platform/DirectX/*.cpp"
"src/Platform/Windows/*.h"
"src/Platform/Windows/*.cpp"
"src/Pixel/Renderer/3D/*.cpp"
"vendor/stb_image/*.h"
"vendor/stb_image/*.cpp"
"vendor/glm/glm/*.hpp"
"vendor/glm/glm/*.inl"
"vendor/entt/include/*.hpp"
"vendor/yaml-cpp/*.h"
"vendor/imguizmo/ImGuizmo.h"
"vendor/imguizmo/ImGuizmo.cpp"
"vendor/Box2D/include/*.h"
"vendor/Box2D/src/*.cpp"
"vendor/assimp/include"
"vendor/imgui-node-editor/imgui_node_editor.h"
"vendor/imgui-node-editor/imgui_node_editor.cpp"
"vendor/imgui-node-editor/imgui_node_editor_internal.h"
"vendor/imgui-node-editor/imgui_node_editor_api.cpp"
"vendor/imgui-node-editor/imgui_bezier_math.h"
"vendor/imgui-node-editor/imgui_canvas.h"
"vendor/imgui-node-editor/imgui_canvas.cpp"
"vendor/imgui-node-editor/imgui_extra_math.h"
"vendor/imgui-node-editor/crude_json.h"
"vendor/imgui-node-editor/crude_json.cpp"
"vendor/lua/*.h"
"vendor/bullet3/src/*.h"
"vendor/bullet3/src/*.cpp"
"vendor/json/include/*.h"
"vendor/json/include/*.cpp"
"vendor/rttr/src/rttr/type"
"vendor/rttr/src/rttr/*.h"
)

set(outputdir "${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME}-${CMAKE_GENERATOR_PLATFORM}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${outputdir}/${PROJECT_NAME})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${outputdir}/${PROJECT_NAME})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${outputdir}/${PROJECT_NAME})

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
"vendor/imguizmo"
"vendor/Box2D/include"
"vendor/assimp/include"
"vendor/imgui-node-editor"
"vendor/lua"
"vendor/bullet3/src"
"vendor/json/include"
"vendor/rttr/src"
"${META_GENERATED_HEADERS}"
)

target_compile_definitions(Pixel
    PRIVATE PX_PLATFORM_WINDOWS
    PRIVATE PX_BUILD_DLL
    PRIVATE GLFW_INCLUDE_NONE
    PRIVATE IMGUI_IMPL_OPENGL_LOADER_CUSTOM
    PRIVATE PX_DEBUG
    PRIVATE YAML_CPP_STATIC_DEFINE
    PRIVATE PIXEL_3D
    PRIVATE __BT_DISABLE_SSE__
    PRIVATE B3_USE_CLEW
    PRIVATE GLM_FORCE_LEFT_HANDED
)

set_source_files_properties(vendor/imguizmo/imGuizmo.cpp PROPERTIES SKIP_PRECOMPILE_HEADERS ON)

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
PUBLIC ${OPENGL_LIBRARIES}
PRIVATE yaml-cpp
PUBLIC ImGui
PUBLIC assimp
PUBLIC lua
PRIVATE "d3d12.lib"
PRIVATE "dxgi.lib"
PRIVATE "d3dcompiler.lib"
PUBLIC RTTR::Core_Lib
PUBLIC ${MODULE_TARGET} # link the reflection module target
)

meta_parser_build(
    TARGET ${MODULE_TARGET} # target name
    SOURCE_ROOT ${META_SOURCE_ROOT} # source root
    GENERATED_DIR ${META_GENERATED_DIR} # generated directory
    GENERATED_FILES ${META_GENERATED_FILES} # generated files
    SOURCE_FILE ${REFLECTION_HEADER} # reflection header
    MODULE_HEADER ${MODULE_HEADER} # module header
    MODULE_SOURCE_FILE ${META_MODULE_SOURCE} # module source
    HEADER_FILES ${HEADERS} # depend header files
    INCLUDES ${REFLECTION_INCLUDES} # -I parameter
    PCH_NAME "pxpch"
    PARSER_EXECUTABLE "$<TARGET_FILE:MetaParser>"
)



