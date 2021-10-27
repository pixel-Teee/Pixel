#pragma once

#include "Pixel/Renderer/VertexArray.h"

namespace Pixel {

	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual const void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override;
		virtual const void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override;

		virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const {return m_VertexBuffers;};
		virtual const Ref<IndexBuffer>& GetIndexBuffer() const { return m_IndexBuffers;};

	private:
		uint32_t m_RendererID;
		std::vector<Ref<VertexBuffer>> m_VertexBuffers;
		Ref<IndexBuffer> m_IndexBuffers;
	};
}
