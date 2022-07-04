#pragma once

#include "Pixel/Renderer/Texture.h"

namespace Pixel {
	class GpuResource;
	class DirectXCubeTexture : public CubeTexture
	{
	public:
		friend class DirectXRenderer;

		DirectXCubeTexture(uint32_t width, uint32_t height, ImageFormat format);

		DirectXCubeTexture(uint32_t width, uint32_t height, ImageFormat format, uint32_t MaxMipLevels);

		virtual ~DirectXCubeTexture();

		virtual Ref<DescriptorHandle> GetRtvHandle(uint32_t index) override;
		virtual Ref<DescriptorHandle> GetSrvHandle() override;
	private:
		Ref<GpuResource> m_pCubeTextureResource;
		Ref<DescriptorHandle> m_RtvHandles[6];
		Ref<DescriptorHandle> m_SrvHandles;

		uint32_t m_Width;
		uint32_t m_Height;
	};
}