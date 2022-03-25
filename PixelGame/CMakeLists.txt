#设置最小版本
cmake_minimum_required(VERSION 3.5)
project(PixelGame)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets
DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB PixelGameSrc
"src/*.h"
"src/*.cpp")

set(outputdir "${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME}-${CMAKE_GENERATOR_PLATFORM}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${outputdir}/${PROJECT_NAME})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${outputdir}/${PROJECT_NAME})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${outputdir}/${PROJECT_NAME})

add_executable(PixelGame ${PixelGameSrc})

target_link_libraries(PixelGame PRIVATE Pixel)

target_include_directories(PixelGame
PRIVATE
"../Pixel/vendor/spdlog/include"
"../Pixel/src"
"../Pixel/vendor"
"../Pixel/vendor/glm"
"../Pixel/vendor/entt/include"
)

target_compile_definitions(PixelGame PRIVATE
PX_PLATFORM_WINDOWS
PX_DEBUG
_CRT_SECURE_NO_WARNINGS
)

target_compile_features(PixelGame
PRIVATE
cxx_std_17
)

if(MSVC)
	set_target_properties(
	PixelGame PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/PixelGame")
endif()



