#include "pxpch.h"
#include "OpenGLUniformBuffer.h"

#include <glad/glad.h>

namespace Pixel {

	OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t Offset, uint32_t BufferSize, uint32_t BindIndex)
	{
		m_Offset = Offset;
		m_BufferSize = BufferSize;
		m_BindIndex = BindIndex;
		glGenBuffers(1, &m_RendererId);
		glBindBuffer(GL_UNIFORM_BUFFER, m_RendererId);
		glBufferData(GL_UNIFORM_BUFFER, BufferSize, nullptr, GL_DYNAMIC_DRAW);
		//glBindBufferBase(GL_UNIFORM_BUFFER, BindIndex, m_RendererId);
		glBindBufferRange(GL_UNIFORM_BUFFER, m_BindIndex, m_RendererId, m_Offset, BufferSize);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void OpenGLUniformBuffer::SetData(uint32_t offset, uint32_t size, void* data)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, m_RendererId);
		glBindBufferRange(GL_UNIFORM_BUFFER, m_BindIndex, m_RendererId, m_Offset, m_BufferSize);
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
		//glBindBuffer(GL_UNIFORM_BUFFER, 0);
		//glBindBufferRange(GL_UNIFORM_BUFFER, BindIndex, m_RendererId, 0, m_BufferSize);
		//glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
}

