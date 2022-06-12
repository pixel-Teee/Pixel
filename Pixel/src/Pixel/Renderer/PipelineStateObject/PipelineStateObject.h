#pragma once

#include "Pixel/Renderer/RendererType.h"

namespace Pixel {
	class Device;
	class RootSignature;
	
	//------many state------
	class BlenderState;
	class RasterState;
	class DepthState;
	//------many state------
	class PSO
	{
	public:
		virtual void SetRootSignature(Ref<RootSignature> pRootSignature) = 0;
		virtual void SetBlendState(Ref<BlenderState> pBlendState) = 0;
		virtual void SetRasterizerState(Ref<RasterState> pRasterState) = 0;
		virtual void SetDepthState(Ref<DepthState> pDepthState) = 0;

		virtual void Finalize() = 0;

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
