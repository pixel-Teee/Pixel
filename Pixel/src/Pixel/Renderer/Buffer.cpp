#include "pxpch.h"
#include "Buffer.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"
#include "Platform/DirectX/Buffer/DirectXVertexBuffer.h"
#include "Platform/DirectX/Buffer/DirectXIndexBuffer.h"

namespace Pixel {

	VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	  PX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:    return new OpenGLVertexBuffer(vertices, size);
		}

		PX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	  PX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:    return CreateRef<OpenGLVertexBuffer>(size);
		}

		PX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t ElementCount, uint32_t ElementSize)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	  PX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::DirectX12:	return CreateRef<DirectXVertexBuffer>(vertices, ElementCount, ElementSize);
		}

		PX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	  PX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:	  return CreateRef<OpenGLIndexBuffer>(indices, count);
		case RendererAPI::API::DirectX12: return CreateRef<DirectXIndexBuffer>(indices, count);
		}

		PX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t count)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	  PX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:	  return CreateRef<OpenGLIndexBuffer>(count);
		}

		PX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	bool BufferLayout::operator==(const BufferLayout& rhs)
	{
		if (m_Elements.size() != rhs.m_Elements.size())
			return false;
		for (size_t i = 0; i < m_Elements.size(); ++i)
		{
			if (m_Elements[i] != rhs.m_Elements[i])
				return false;
		}
		return true;
	}

	bool BufferElement::operator==(const BufferElement& rhs)
	{
		return Name == rhs.Name && Type == rhs.Type && Offset == rhs.Offset &&
			Size == rhs.Size && Normalized == rhs.Normalized && m_sematics == rhs.m_sematics;
	}

}