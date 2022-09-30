#pragma  once

#include <glm/glm.hpp>

namespace Pixel {

	namespace Math {
		bool DecomposeTransform(const glm::mat4& tranform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale);

		//mask is (2^n - 1)
		template<typename T> 
		T AlignUpWithMask(T value, size_t mask)
		{
			return (T)(((size_t)value + mask) & ~mask);
		}

		template<typename T>
		T AlignDownWithMask(T value, size_t mask)
		{
			return (T)((size_t)value & ~mask);
		}

		//2^n - 1
		template<typename T>
		T AlignUp(T value, size_t alignment)
		{
			return AlignUpWithMask(value, alignment - 1);
		}

		template<typename T>
		T AlignDown(T value, size_t alignment)
		{
			return AlignDownWithMask(value, alignment - 1);
		}

		template<typename T>
		bool IsAligned(T value, size_t alignment)
		{
			return ((size_t)value & (alignment - 1)) == 0;
		}

		//Ceiling
		template<typename T>
		T DivideByMultiple(T value, size_t alignment)
		{
			return (T)((value + alignment - 1) / alignment);
		}

		template<typename T>
		bool IsPowerOfTwo(T value)
		{
			return (value & (value - 1)) == 0;
		}

		template<typename T>
		bool IsDivisible(T value, T divisor)
		{
			return (value / divisor) * divisor == value;
		}
	}
}