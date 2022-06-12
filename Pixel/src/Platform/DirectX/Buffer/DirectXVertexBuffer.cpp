#include "pxpch.h"
#include "DirectXVertexBuffer.h"

namespace Pixel {

	DirectXVertexBuffer::DirectXVertexBuffer(float* vertices, uint32_t ByteSize)
	{
		//get the device and command list

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
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DirectXVertexBuffer::SetLayout(const BufferLayout& layout)
	{
		throw std::logic_error("The method or operation is not implemented.");
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