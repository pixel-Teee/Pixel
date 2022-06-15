#pragma once

#include "DirectXPixelBuffer.h"

namespace Pixel {
	class Device;
	//class DescriptorCpuHandle;
	class DirectXDescriptorCpuHandle;
	class DepthBuffer : public DirectXPixelBuffer
	{
	public:
		//DepthBuffer();
		DepthBuffer(float ClearDepth = 0.0f, uint8_t ClearStencil = 0);

		void Create(const std::wstring& Name, uint32_t Width, uint32_t Height,
			ImageFormat Format, Ref<GpuVirtualAddress> VideoMemoryPtr /*= -1*/);

		void Create(const std::wstring& Name, uint32_t Width, uint32_t Height, uint32_t NumSamples, ImageFormat Format,
			Ref<GpuVirtualAddress> VideoMemoryPtr);

		Ref<DescriptorCpuHandle> GetDSV();
		Ref<DescriptorCpuHandle> GetDSV_DepthReadOnly();
		Ref<DescriptorCpuHandle> GetDSV_StencilReadOnly();
		Ref<DescriptorCpuHandle> GetDSV_ReadOnly();
		Ref<DescriptorCpuHandle> GetDepthSRV();
		Ref<DescriptorCpuHandle> GetStencilSRV();

		float GetClearDepth() const { return m_ClearDepth; }
		uint8_t GetClearStencil() const { return m_ClearStencil; }

	protected:

		void CreateDerivedViews(DXGI_FORMAT Format);

		float m_ClearDepth;
		uint8_t m_ClearStencil;
		Ref<DirectXDescriptorCpuHandle> m_hDSV[4];
		Ref<DirectXDescriptorCpuHandle> m_hDepthSRV;
		Ref<DirectXDescriptorCpuHandle> m_hStencilSRV;
	};
}