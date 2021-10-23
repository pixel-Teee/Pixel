workspace "Pixel"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}
	startproject "SandBox"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

--Include directories relative to root folder(solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "Pixel/vendor/GLFW/include"
IncludeDir["Glad"] = "Pixel/vendor/Glad/include"
IncludeDir["ImGui"] = "Pixel/vendor/imgui"
IncludeDir["glm"] = "Pixel/vendor/glm"

include "Pixel/vendor/GLFW"
include "Pixel/vendor/Glad"
include "Pixel/vendor/imgui"

project "Pixel"
	location "Pixel"
	kind "StaticLib"
	cppdialect "C++17"
	language "C++"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "pxpch.h"
	pchsource "Pixel/src/pxpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}"
	}

	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
		systemversion "10.0"

		defines
		{
			"PX_PLATFORM_WINDOWS",
			"PX_BUILD_DLL",
			"GLFW_INCLUDE_NONE",
			"IMGUI_IMPL_OPENGL_LOADER_CUSTOM"
		}

	filter "configurations:Debug"
		defines "PX_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "PX_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "PX_DIST"
		runtime "Release"
		optimize "on"

project "SandBox"
	location "SandBox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/"..outputdir.."/%{prj.name}")
	objdir("bin-int/"..outputdir.."/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Pixel/vendor/spdlog/include",
		"Pixel/src",
		"Pixel/vendor",
		"%{IncludeDir.glm}"
	}

	links
	{
		"Pixel"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"PX_PLATFORM_WINDOWS"
		}

		filter "configurations:Debug"
			defines "PX_DEBUG"
			runtime "Debug"
			symbols "on"

		filter "configurations:Release"
			defines "PX_RELEASE"
			runtime "Release"
			optimize "on"

		filter "configurations:Dist"
			defines "PX_DIST"
			runtime "Release"
			optimize "on"