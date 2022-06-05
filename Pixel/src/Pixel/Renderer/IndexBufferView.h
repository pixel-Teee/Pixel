#pragma once

namespace Pixel {
	class GpuVirtualAddress;
	class IBV
	{
	public:

		static Ref<IBV> Create(Ref<GpuVirtualAddress> pGpuVirtualAddress, size_t OffSet, uint32_t Size, bool b32Bit);
	};
}
