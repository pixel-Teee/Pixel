#pragma once

#include "Pixel/Renderer/Texture.h"

namespace Pixel {
	class DirectXColorBuffer;
	class DirectXGpuResource;
	class DescriptorCpuHandle;
	class Device;
	class DescriptorHandle;
	class DirectXTexture : public Texture2D
	{
	public:
		friend class Context;
		//DirectXTexture(uint32_t width, uint32_t height, ImageFormat textureFormat);
		DirectXTexture(uint32_t RowPitch, uint32_t width, uint32_t height, ImageFormat textureFormat, const void*
		InitialData);
		DirectXTexture(const std::string& path);

		virtual bool operator ==(const Texture& other) const override
		{
			//throw std::logic_error("The method or operation is not implemented.");
			return false;
		}

		virtual uint32_t GetWidth() const override;

		virtual uint32_t GetHeight() const override;

		virtual uint64_t GetRendererID() const override;

		virtual Ref<DescriptorCpuHandle> GetCpuDescriptorHandle() override;
		virtual Ref<DescriptorHandle> GetHandle() const override;

		virtual Ref<DescriptorHandle> GetRtvHandle() const override;

		//need to refractor
		virtual void SetData(void* data, uint32_t size) override;

		virtual void Bind(uint32_t slot = 0) const override;

		virtual std::string& GetPath() override;

		std::string m_path;
		uint32_t m_Width, m_Height;

		Ref<DirectXGpuResource> m_pGpuResource;

		Ref<DescriptorHandle> m_pHandle;

		Ref<DescriptorHandle> m_RtvHandle;
	};
}
