#pragma once

namespace Pixel {
	namespace Utility {

		inline size_t HashRange(const uint32_t* const Begin, const uint32_t* const End, size_t Hash);

		//third number is hash code
		template<typename T>
		inline size_t HashState(const T* StateDesc, size_t Count = 1, size_t Hash = 2166136261U)
		{
			//sizeof(T) & 3: T is 4's multipile
			static_assert((sizeof(T) & 3) == 0 && alignof(T) >= 4, "State object is not word-aligned!");

			return HashRange((uint32_t*)StateDesc, (uint32_t*)(StateDesc + Count), Hash);
		}

	}
}
