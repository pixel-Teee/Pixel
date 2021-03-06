#pragma once

#include "Pixel/Renderer/RendererType.h"

namespace Pixel {

	class RootParameter;
	class SamplerDesc;
	class Device;
	class RootSignature {
	public:
		virtual void Reset(uint32_t NumRootParams, uint32_t NumStaticSamplers) = 0;

		virtual RootParameter& operator[](size_t index) = 0;

		virtual void InitStaticSampler(uint32_t Register, Ref<SamplerDesc> NonStaticSamplerDesc, ShaderVisibility Visibility = ShaderVisibility::ALL) = 0;

		virtual void Finalize(const std::wstring& name, RootSignatureFlag Flags) = 0;

		static Ref<RootSignature> Create(uint32_t NumRootParams, uint32_t NumStaticSampler);
	};
}