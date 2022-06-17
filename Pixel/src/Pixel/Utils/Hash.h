#pragma once

namespace Pixel {
	namespace Utility {

		size_t HashRange(const uint32_t* const Begin, const uint32_t* const End, size_t Hash);

		//template<typename T>
		size_t HashState(const uint32_t* StateDesc, size_t Count = 1, size_t Hash = 2166136261U);

	}
}
