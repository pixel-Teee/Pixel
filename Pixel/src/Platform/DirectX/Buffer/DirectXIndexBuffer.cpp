#include "pxpch.h"
#include "DirectXIndexBuffer.h"

#include "Platform/DirectX/Buffer/DirectXGpuBuffer.h"
#include "Platform/DirectX/View/DirectXIndexBufferView.h"

namespace Pixel {

	DirectXIndexBuffer::DirectXIndexBuffer(uint32_t* indices, uint32_t count)
	{
		m_pResource = CreateRef<DirectXByteAddressBuffer>();
		std::static_pointer_cast<DirectXByteAddressBuffer>(m_pResource)->Create(L"IndexBuffer", count, sizeof(uint32_t), indices);

		m_pIBV = CreateRef<DirectXIBV>(std::static_pointer_cast<DirectXByteAddressBuffer>(m_pResource)->GetGpuVirtualAddress(),
			0, count * sizeof(uint32_t), true);

		m_Count = count;
	}

	void DirectXIndexBuffer::Bind() const
	{

	}

	void DirectXIndexBuffer::Unbind() const
	{

	}

	void DirectXIndexBuffer::SetData(const void* data, uint32_t count)
	{

	}

	uint32_t DirectXIndexBuffer::GetCount() const
	{
		return m_Count;
	}

	Ref<IBV> DirectXIndexBuffer::GetIBV()
	{
		return m_pIBV;
	}

}
