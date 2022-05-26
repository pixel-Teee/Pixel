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

	}
}