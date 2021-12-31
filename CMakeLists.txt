#设置最小版本
cmake_minimum_required(VERSION 3.5)
#解决方案名称
project(Pixel)

#设置输出目录
#例如Debug-Windows-x64
set(outputdir "${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME}-${CMAKE_GENERATOR_PLATFORM}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/${outputdir}/${PROJECT_NAME})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/${outputdir}/${PROJECT_NAME})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/${outputdir}/${PROJECT_NAME})

add_subdirectory(Pixel/vendor/Glad)
add_subdirectory(Pixel/vendor/GLFW)
add_subdirectory(Pixel/vendor/imgui)
add_subdirectory(Pixel)
add_subdirectory(SandBox)
add_subdirectory(Pixel-Editor)

target_compile_definitions(Pixel
    PRIVATE _CRT_SECURE_NO_WARNINGS
)
target_compile_features(Pixel
    PUBLIC cxx_std_17
)

