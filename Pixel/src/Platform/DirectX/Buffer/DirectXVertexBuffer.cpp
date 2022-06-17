#include "pxpch.h"
#include "DirectXVertexBuffer.h"

#include "Platform/DirectX/Buffer/DirectXGpuBuffer.h"
#include "Platform/DirectX/View/DirectXVertexBufferView.h"

namespace Pixel {

	//TODO:need to complete
	DirectXVertexBuffer::DirectXVertexBuffer(float* vertices, uint32_t ByteSize)
	{
		//get the device and command list
		m_pResource = CreateRef<DirectXByteAddressBuffer>();
		//std::static_pointer_cast<DirectXByteAddressBuffer>(m_pResource)->Create(L"VertexBuffer", , ByteSize, );
	}

	DirectXVertexBuffer::DirectXVertexBuffer(float* vertices, uint32_t ElementCount, uint32_t ElementSize)
	{
		m_pResource = CreateRef<DirectXByteAddressBuffer>();
		std::static_pointer_cast<DirectXByteAddressBuffer>(m_pResource)->Create(L"VertexBuffer", ElementCount, ElementSize, vertices);

		m_pVBV = CreateRef<DirectXVBV>(std::static_pointer_cast<DirectXByteAddressBuffer>(m_pResource)->GetGpuVirtualAddress(),
		0, ElementCount * ElementSize, ElementSize);
	}

	Ref<VBV> DirectXVertexBuffer::GetVBV()
	{
		return m_pVBV;
	}

	void DirectXVertexBuffer::Bind() const
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DirectXVertexBuffer::Unbind() const
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DirectXVertexBuffer::SetData(const void* data, uint32_t size)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	const Pixel::BufferLayout& DirectXVertexBuffer::GetLayout() const
	{
		return m_Layout;
	}

	void DirectXVertexBuffer::SetLayout(const BufferLayout& layout)
	{
		m_Layout = layout;
	}

	bool DirectXVertexBuffer::HavePosition()
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	bool DirectXVertexBuffer::HaveNormal()
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	bool DirectXVertexBuffer::HaveTangent()
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	bool DirectXVertexBuffer::HaveBinormal()
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	bool DirectXVertexBuffer::HaveColors(uint32_t Level)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	bool DirectXVertexBuffer::HaveTexCoord(uint32_t Level)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	bool DirectXVertexBuffer::HaveBoneIndex(uint32_t Level)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	bool DirectXVertexBuffer::HaveBoneWeight(uint32_t Level)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

}