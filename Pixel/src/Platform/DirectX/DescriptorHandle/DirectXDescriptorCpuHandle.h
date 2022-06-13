#pragma once

#include "Platform/DirectX/d3dx12.h"
#include "Pixel/Renderer/DescriptorHandle/DescriptorCpuHandle.h"

namespace Pixel {
	class DirectXDescriptorCpuHandle : public DescriptorCpuHandle
	{
	public:
		DirectXDescriptorCpuHandle();
		DirectXDescriptorCpuHandle(const DirectXDescriptorCpuHandle& rhs) { m_CpuHandle = rhs.m_CpuHandle; }

		//D3D12_CPU_DESCRIPTOR_HANDLE
		virtual void SetCpuHandle(void* handle) override { m_CpuHandle = *((D3D12_CPU_DESCRIPTOR_HANDLE*)handle); }
		void SetCpuHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle) { m_CpuHandle = handle; }
		D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle() { return m_CpuHandle; }
		operator D3D12_CPU_DESCRIPTOR_HANDLE() const { return m_CpuHandle; }
		
		virtual size_t GetCpuPtr() const override;
		virtual bool IsNull() const override;
		virtual DescriptorCpuHandle& operator+= (int32_t OffsetScaledByDescriptorSize) override;
		DirectXDescriptorCpuHandle operator+ (int32_t OffsetScaledByDescriptorSize);
	private:
		D3D12_CPU_DESCRIPTOR_HANDLE m_CpuHandle;
	};
}
