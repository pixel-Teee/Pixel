#include "pxpch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Pixel {
	static GLenum ShaderDataTypeToOpenGLBasetype(ShaderDataType type)
	{
		switch (type)
		{
		case Pixel::ShaderDataType::None:	return GL_FLOAT;
		case Pixel::ShaderDataType::Float:	return GL_FLOAT;
		case Pixel::ShaderDataType::Float2:	return GL_FLOAT;
		case Pixel::ShaderDataType::Float3:	return GL_FLOAT;
		case Pixel::ShaderDataType::Float4:	return GL_FLOAT;
		case Pixel::ShaderDataType::Mat3:	return GL_FLOAT;
		case Pixel::ShaderDataType::Mat4:	return GL_FLOAT;
		case Pixel::ShaderDataType::Int:	return GL_INT;
		case Pixel::ShaderDataType::Int2:	return GL_INT;
		case Pixel::ShaderDataType::Int3:	return GL_INT;
		case Pixel::ShaderDataType::Int4:	return GL_INT;
		case Pixel::ShaderDataType::Bool:	return GL_BOOL;
		}
		PX_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		glCreateVertexArrays(1, &m_RendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}

	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(m_RendererID);
	}

	void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	const void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();

		PX_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");

		uint32_t index = 0;
		const auto& layout = vertexBuffer->GetLayout();
		for (const auto& element : layout)
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index,
				element.GetComponentCount(),
				ShaderDataTypeToOpenGLBasetype(element.Type),
				element.Normalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				(const void*)element.Offset);
			++index;
		}

		m_VertexBuffers.push_back(vertexBuffer);
	}

	const void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();

		m_IndexBuffers = indexBuffer;
	}
}

