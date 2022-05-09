#pragma once

namespace Pixel {
	class UniformBuffer {
	public:
		virtual ~UniformBuffer() = default;

		virtual void SetData(uint32_t offset, uint32_t size, void* data) = 0;

		//BindIndex
		static Ref<UniformBuffer> Create(uint32_t Offset, uint32_t BufferSize, uint32_t BindIndex);
	};

	namespace Utils {
		template<uint32_t alignment>
		uint32_t Helper(uint32_t offset)
		{
			uint32_t leftByte = offset % alignment;

			if (leftByte)
			{
				return offset + (alignment - leftByte);
			}
			else
			{
				return offset;
			}
		}

		template<uint32_t alignment>
		void CalculateTypeOffsetAndSize(uint32_t offset, std::vector<uint32_t>& outRes)
		{

		}

		template<uint32_t alignment, typename T, typename... Args>
		void CalculateTypeOffsetAndSize(uint32_t offset, std::vector<uint32_t>& outRes)
		{
			uint32_t newOffset = Helper<alignment>(offset);

			outRes.push_back(newOffset);

			//printf("%d\n", newOffset);

			CalculateTypeOffsetAndSize<alignment, Args...>(newOffset + sizeof(T), outRes);
		}
	}
}