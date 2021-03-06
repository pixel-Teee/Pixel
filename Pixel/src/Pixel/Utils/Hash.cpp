#include "pxpch.h"
#include "Hash.h"

namespace Pixel {
	namespace Utility {

		size_t HashRange(const uint32_t* const Begin, const uint32_t* const End, size_t Hash)
		{
			for (const uint32_t* Iter = Begin; Iter < End; ++Iter)
				Hash = 16777619U * Hash ^ *Iter;

			return Hash;
		}

		//template<typename T>
		size_t HashState(const uint32_t* StateDesc, size_t Count, size_t Hash)
		{
			//sizeof(T) & 3: T is 4's multipile
			//static_assert((sizeof(T) & 3) == 0 && alignof(T) >= 4, "State object is not word-aligned!");

			return HashRange(StateDesc, (uint32_t*)(StateDesc + Count), Hash);
		}
	}
}