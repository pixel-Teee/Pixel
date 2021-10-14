#pragma once

#ifdef PX_PLATFORM_WINDOWS
	#ifdef PX_BUILD_DLL
		#define PIXEL_API __declspec(dllexport)
	#else
		#define PIXEL_API __declspec(dllimport)
	#endif
#else
	#error Pixel only support Windows!
#endif

#define BIT(x) (1 << x)

#ifdef PX_ENABLE_ASSERTS
	#define PX_ASSERT(x, ...) { if(!x) {PX_ERROR("Assertion Failed: {0}", __VA_ARGS__); _debugbreak();}}
	#define PX_CORE_ASSERT(x, ...) { if(!x){PX_CORE_ERROR("Assertion Failed:{0}", __VA_ARGS__); _debugbreak();}}
#else
	#define PX_ASSERT(x, ...)
	#define PX_CORE_ASSERT(x, ...)
#endif
