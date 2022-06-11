#include "pxpch.h"

#ifdef PX_OPENGL
#include "backends/imgui_impl_opengl3.cpp"
#include "backends/imgui_impl_glfw.cpp"
#else
#include "backends/imgui_impl_dx12.cpp"
#include "backends/imgui_impl_glfw.cpp"
#endif