#pragma once

#include "Pixel/Renderer/Buffer/PixelBuffer.h"

namespace Pixel {
	class RootSignature;
	class Device;
	class PSO
	{
	public:
		virtual void SetRootSignature(Ref<RootSignature> pRootSignature) = 0;

		virtual void Finalize(Ref<Device> pDevice) = 0;

		virtual void SetDepthTargetFormat(ImageFormat DSVFormat, uint32_t MsaaCount = 1, uint32_t MsaaQuality = 0) = 0;

		virtual void SetRenderTargetFormat(ImageFormat RTVFormat, ImageFormat DSVFormat, uint32_t MsaaCount = 1, uint32_t MsaaQuality = 0) = 0;

		virtual void SetRenderTargetFormats(uint32_t NumRTVs, const ImageFormat* RTVFormats, ImageFormat DSVFormat, uint32_t MsaaCount = 1,
			uint32_t MsaaQuality = 0) = 0;

		//virtual void SetInputLayout()

		virtual void SetVertexShader(const void* Binary, size_t Size) = 0;

		virtual void SetPixelShader(const void* Binary, size_t Size) = 0;

		static Ref<PSO> CreateGraphicsPso(const wchar_t* Name);
	};
}
