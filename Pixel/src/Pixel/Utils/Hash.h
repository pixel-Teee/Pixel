#pragma once

namespace Pixel {
	namespace Utility {

		size_t HashRange(const uint32_t* const Begin, const uint32_t* const End, size_t Hash);

		//template<typename T>
		//size_t HashState(const uint32_t* StateDesc, size_t Count = 1, size_t Hash = 2166136261U);

		template<typename T>
		size_t HashState(const T* StateDesc, size_t Count = 1, size_t Hash = 2166136261U)
		{
			static_assert((sizeof(T) & 3) == 0 && alignof(T) >= 4, "state object is not word-aligned!");
			return HashRange((uint32_t*)StateDesc, (uint32_t*)(StateDesc + Count), Hash);
		}
	}
}
