#pragma once

#include "Pixel/Renderer/Buffer.h"

namespace Pixel {
	
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(uint32_t size);
		OpenGLVertexBuffer(float* vertices, uint32_t size);
		virtual ~OpenGLVertexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual void SetData(const void* data, uint32_t size) override;
	
		virtual const BufferLayout& GetLayout() const override{ return m_Layout; }
		virtual void SetLayout(const BufferLayout& layout) override{ m_Layout = layout;}

		//------check vertex type info------
		bool HavePosition() override;
		bool HaveNormal() override;
		bool HaveTangent() override;
		bool HaveBinormal() override;
		//may have levels vertex color
		bool HaveColors(uint32_t Level) override;
		//may have levels vertex texcoord
		bool HaveTexCoord(uint32_t Level) override;

		bool HaveBoneIndex(uint32_t Level) override;
		bool HaveBoneWeight(uint32_t Level) override;
		//------check vertex type info------
	private:
		bool CheckHaveSematics(Semantics semantics, uint32_t level);
		uint32_t m_RendererID;
		BufferLayout m_Layout;
		//------data------
		unsigned char* m_data;
		uint32_t m_size;
		//------data------
	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
		OpenGLIndexBuffer(uint32_t count);
		virtual ~OpenGLIndexBuffer();

		virtual void Bind() const;
		virtual void Unbind() const;
		virtual void SetData(const void* data, uint32_t count) override;
		
		virtual uint32_t GetCount() const { return m_Count; }
	private:
		uint32_t m_RendererID;
		uint32_t m_Count;
	};
}
