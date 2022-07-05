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

		virtual Ref<DescriptorHandle> GetRtvHandle(uint32_t mipSlice, uint32_t arraySlice) override;
		virtual Ref<DescriptorHandle> GetSrvHandle() override;
		virtual Ref<DescriptorHandle> GetUavHandle(uint32_t mipSlice, uint32_t arraySlice) override;
	private:
		Ref<GpuResource> m_pCubeTextureResource;
		Ref<DescriptorHandle> m_RtvHandles[30];
		Ref<DescriptorHandle> m_SrvHandles;

		Ref<DescriptorHandle> m_TextureUAVHandles[24];

		uint32_t m_Width;
		uint32_t m_Height;
		uint32_t m_mipLevels = 1;
		uint32_t m_arraySize = 6;
	};
}