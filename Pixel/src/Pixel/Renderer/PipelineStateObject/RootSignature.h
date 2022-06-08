#pragma once

namespace Pixel {
	class RootSignature {
	public:
		virtual void Reset(uint32_t NumRootParams, uint32_t NumStaticSamplers) = 0;

		static Ref<RootSignature> Create(uint32_t NumRootParams, uint32_t NumStaticSampler);
	};
}