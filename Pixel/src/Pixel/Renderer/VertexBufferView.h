#pragma once

namespace Pixel {
	class GpuVirtualAddress;
	class VBV
	{
	public:

		static Ref<VBV> Create(Ref<GpuVirtualAddress> pGpuVirtualAddress, size_t OffSet, uint32_t Size, uint32_t Stride);
	};
}
