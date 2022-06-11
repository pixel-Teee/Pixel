#pragma once

namespace Pixel {
	enum class RootBindings
	{
		MeshConstants,
		MaterialConstants,
		MaterialSRVs,
		MaterialSamplers,
		CommonSRVs,
		CommonCBV,
		SkinMatrices,
		NumRootBindings
	};

	class RootParameter;

	class RootSignature {
	public:
		virtual void Reset(uint32_t NumRootParams, uint32_t NumStaticSamplers) = 0;

		virtual RootParameter& operator[](size_t index) = 0;

		static Ref<RootSignature> Create(uint32_t NumRootParams, uint32_t NumStaticSampler);
	};
}