#pragma once

#include "DepthBuffer.h"

namespace Pixel {
	class ShadowBuffer : public DepthBuffer
	{
	public:
		ShadowBuffer();

		void Create(const std::wstring& Name, uint32_t Width, uint32_t Height,
			Ref<GpuVirtualAddress> VideoMemoryPtr);

		void Create(const std::wstring& Name, uint32_t Width, uint32_t Height);

		Ref<DescriptorCpuHandle> GetSRV() { return GetDepthSRV(); }

		void BeginRendering(GraphicsContext& context);
		void EndRendering(GraphicsContext& context);

	private:
		ViewPort m_ViewPort;
		PixelRect m_Scissor;
	};
}