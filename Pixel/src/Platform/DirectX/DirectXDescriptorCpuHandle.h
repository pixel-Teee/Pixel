#pragma once

#include "d3dx12.h"
#include "Pixel/Renderer/DescriptorCpuHandle.h"

namespace Pixel {
	class DirectXDescriptorCpuHandle : public DescriptorCpuHandle
	{
	public:
		DirectXDescriptorCpuHandle();

		void SetCpuHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle) { m_CpuHandle = handle; }
		D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle() { return m_CpuHandle; }
		
		virtual size_t GetCpuPtr() const override;
		virtual bool IsNull() const override;

		operator D3D12_CPU_DESCRIPTOR_HANDLE() const { return m_CpuHandle; }
		virtual DescriptorCpuHandle& operator+= (int32_t OffsetScaledByDescriptorSize) override;
		DescriptorCpuHandle& operator+ (int32_t OffsetScaledByDescriptorSize);
	private:
		D3D12_CPU_DESCRIPTOR_HANDLE m_CpuHandle;
	};
}
