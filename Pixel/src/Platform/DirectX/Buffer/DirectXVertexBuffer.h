#pragma once

#include <wrl/client.h>

#include "Platform/DirectX/d3dx12.h"
#include "Pixel/Renderer/Buffer.h"

namespace Pixel {
	class GpuResource;
	class VBV;
	//TODO:need to inherit from vertex buffer
	class DirectXVertexBuffer : public VertexBuffer 
	{
	public:
		DirectXVertexBuffer(float* vertices, uint32_t ElementCount, uint32_t ElementSize);
		DirectXVertexBuffer(float* vertices, uint32_t ByteSize);

		//return vertex view
		virtual Ref<VBV> GetVBV() override;

		virtual void Bind() const override;

		virtual void Unbind() const override;

		virtual void SetData(const void* data, uint32_t size) override;

		virtual const BufferLayout& GetLayout() const override;

		virtual void SetLayout(const BufferLayout& layout) override;

		virtual bool HavePosition() override;

		virtual bool HaveNormal() override;

		virtual bool HaveTangent() override;

		virtual bool HaveBinormal() override;

		virtual bool HaveColors(uint32_t Level) override;

		virtual bool HaveTexCoord(uint32_t Level) override;

		virtual bool HaveBoneIndex(uint32_t Level) override;

		virtual bool HaveBoneWeight(uint32_t Level) override;
	private:
		BufferLayout m_Layout;
		Ref<GpuResource> m_pResource;
		Ref<VBV> m_pVBV;

		uint32_t m_ElementCount;
		uint32_t m_ElementSize;
	};
}
