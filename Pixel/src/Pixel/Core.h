#pragma once

#include <memory>

#ifdef PX_PLATFORM_WINDOWS
#if PX_DYNAMIC_LINK
	#ifdef PX_BUILD_DLL
		#define PIXEL_API __declspec(dllexport)
	#else
		#define PIXEL_API __declspec(dllimport)
	#endif
#else
	#define PIXEL_API
#endif
#else
	#error Pixel only support Windows!
#endif

#define BIT(x) (1 << x)

#ifdef PX_DEBUG
	#define PX_ENABLE_ASSERTS
#endif

#ifdef PX_ENABLE_ASSERTS
	#define PX_ASSERT(x, ...) { if(!x) {PIXEL_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak();}}
	#define PX_CORE_ASSERT(x, ...) { if(!(x)) { PIXEL_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define PX_ASSERT(x, ...)
	#define PX_CORE_ASSERT(x, ...)
#endif

#define PX_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace Pixel {

	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T>
	using Ref = std::shared_ptr<T>;

	//Ref<Shader>
	//using ShaderRef = Ref<Shader>;
}
