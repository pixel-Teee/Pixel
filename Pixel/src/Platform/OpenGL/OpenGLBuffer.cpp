#include "pxpch.h"
#include "OpenGLBuffer.h"

#include <glad/glad.h>

namespace Pixel {
	
	///////////////////////////
	//VertexBuffer////////////
	///////////////////////////

	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size)
	{
		glCreateBuffers(1, &m_RendererID);	
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
	{
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLVertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	}

	void OpenGLVertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void OpenGLVertexBuffer::SetData(const void* data, uint32_t size)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}

	bool OpenGLVertexBuffer::HavePosition()
	{
		return CheckHaveSematics(Semantics::POSITION, 1);
	}

	bool OpenGLVertexBuffer::HaveNormal()
	{
		return CheckHaveSematics(Semantics::NORMAL, 1);
	}

	bool OpenGLVertexBuffer::HaveTangent()
	{
		return CheckHaveSematics(Semantics::TANGENT, 1);
	}

	bool OpenGLVertexBuffer::HaveBinormal()
	{
		return CheckHaveSematics(Semantics::BINORMAL, 1);
	}

	bool OpenGLVertexBuffer::HaveColors(uint32_t Level)
	{
		return CheckHaveSematics(Semantics::COLOR, Level);
	}

	bool OpenGLVertexBuffer::HaveTexCoord(uint32_t Level)
	{
		return CheckHaveSematics(Semantics::TEXCOORD, 1);
	}

	bool OpenGLVertexBuffer::HaveBoneIndex(uint32_t Level)
	{
		return CheckHaveSematics(Semantics::BLENDINDICES, Level);
	}

	bool OpenGLVertexBuffer::HaveBoneWeight(uint32_t Level)
	{
		return CheckHaveSematics(Semantics::BLENDWEIGHT, Level);
	}

	Ref<VBV> OpenGLVertexBuffer::GetVBV()
	{
		return nullptr;
	}

	bool OpenGLVertexBuffer::CheckHaveSematics(Semantics semantics, uint32_t level)
	{
		uint32_t Level = 0;
		for (auto it = m_Layout.begin(); it != m_Layout.end(); ++it)
		{
			if (it->m_sematics == semantics)
			{
				++Level;
			}
		}
		if (Level >= level)
		{
			return true;
		}
		return false;
	}

	///////////////////////////
	//IndicesBuffer////////////
	///////////////////////////

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count):m_Count(count)
	{
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t count)
	{
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * count, nullptr, GL_DYNAMIC_DRAW);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLIndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	}

	void OpenGLIndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void OpenGLIndexBuffer::SetData(const void* data, uint32_t count)
	{
		m_Count = count;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(uint32_t) * count, data);
	}

	Ref<IBV> OpenGLIndexBuffer::GetIBV()
	{
		//throw std::logic_error("The method or operation is not implemented.");
		return nullptr;
	}

}