#设置最小版本
cmake_minimum_required(VERSION 3.5)
#解决方案名称
project(Pixel)

add_definitions(-DUNICODE -D_UNICODE)

add_subdirectory(Pixel/vendor/Glad)
add_subdirectory(Pixel/vendor/GLFW)
add_subdirectory(Pixel/vendor/imgui)
add_subdirectory(Pixel/vendor/yaml-cpp)
add_subdirectory(Pixel/vendor/assimp)
add_subdirectory(Pixel/vendor/lua)
add_subdirectory(Pixel/vendor/rttr)
add_subdirectory(Pixel)
add_subdirectory(SandBox)
add_subdirectory(PixelEditor)
add_subdirectory(PixelGame)
add_subdirectory(MetaParser MetaParser)

target_compile_definitions(Pixel
    PRIVATE _CRT_SECURE_NO_WARNINGS
)
target_compile_features(Pixel
    PUBLIC cxx_std_17
)

