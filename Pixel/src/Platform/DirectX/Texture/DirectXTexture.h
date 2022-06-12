#pragma once

#include "Pixel/Renderer/Texture.h"

namespace Pixel {
	class DirectXColorBuffer;
	class DirectXGpuResource;
	class DescriptorCpuHandle;
	class Device;
	class DirectXTexture : public Texture2D
	{
	public:
		DirectXTexture(uint32_t RowPitch, uint32_t width, uint32_t height, ImageFormat textureFormat, Ref<ContextManager> pContextManager, Ref<Device> pDevice);
		DirectXTexture(const std::string& path);

		virtual bool operator ==(const Texture& other) const override
		{
			//throw std::logic_error("The method or operation is not implemented.");
			return false;
		}

		virtual uint32_t GetWidth() const override;

		virtual uint32_t GetHeight() const override;

		virtual uint32_t GetRendererID() const override;

		//need to refractor
		virtual void SetData(void* data, uint32_t size) override;

		virtual void Bind(uint32_t slot = 0) const override;

		virtual std::string& GetPath() override;
	
	private:
		std::string m_path;
		uint32_t m_Width, m_Height;

		Ref<DirectXGpuResource> m_pGpuResource;

		Ref<DescriptorCpuHandle> m_pCpuDescriptorHandle;
	};
}
