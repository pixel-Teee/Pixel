#pragma once

#include "Pixel/Renderer/Buffer.h"

namespace Pixel {
	class GpuResource;
	class IBV;
	class DirectXIndexBuffer : public IndexBuffer
	{
	public:
		DirectXIndexBuffer(uint32_t* indices, uint32_t count);

		virtual void Bind() const override;

		virtual void Unbind() const override;

		virtual void SetData(const void* data, uint32_t count) override;

		virtual uint32_t GetCount() const override;

		virtual Ref<IBV> GetIBV() override;
	private:
		Ref<GpuResource> m_pResource;
		Ref<IBV> m_pIBV;

		uint32_t m_Count;
	};
}
