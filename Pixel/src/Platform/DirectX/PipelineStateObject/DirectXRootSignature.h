#pragma once

#include "Platform/DirectX/Context/DirectXContext.h"
#include "Pixel/Renderer/PipelineStateObject/RootSignature.h"
#include "Pixel/Renderer/PipelineStateObject/RootParameter.h"
#include "Pixel/Renderer/RendererType.h"

namespace Pixel { 

	class SamplerDesc;
	class DirectXRootSignature : public RootSignature
	{
		friend class DirectXDynamicDescriptorHeap;
		friend class DescriptorHandleCache;
	public:
		//root parameter's number and static sampler's number
		DirectXRootSignature(uint32_t NumRootParams = 0, uint32_t NumStaticSampler = 0);

		virtual void Reset(uint32_t NumRootParams, uint32_t NumStaticSamplers = 0) override;

		//------access array------
		virtual RootParameter& operator[](size_t EntryIndex) override;
		const RootParameter& operator[](size_t EntryIndex) const;
		//------access array------

		void InitStaticSampler(uint32_t Register, const D3D12_SAMPLER_DESC& NonStaticSamplerDesc,
			D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL);

		virtual void InitStaticSampler(uint32_t Register, Ref<SamplerDesc> NonStaticSamplerDesc, ShaderVisibility Visibility = ShaderVisibility::ALL) override;

		//D3D12_ROOT_SIGNATURE_FLAG_NONE
		//flags:flag counld input assembly etc.
		virtual void Finalize(const std::wstring& name, RootSignatureFlag Flags, Ref<Device> pDevice) override;

		ID3D12RootSignature* GetNativeSignature() const;
	protected:
		//have already initialized?
		bool m_finalized;

		uint32_t m_NumParameters;

		uint32_t m_NumSamplers;

		//alreay initialized's static samplers
		uint32_t m_NumInitializedStaticSamplers;

		//root parameter array
		std::vector<Ref<RootParameter>> m_ParamArray;
		//static sampler desc array
		std::unique_ptr<D3D12_STATIC_SAMPLER_DESC[]> m_SamplerArray;

		ID3D12RootSignature* m_Signature;

		uint32_t m_DescriptorTableBitMap; //one bit is set for root parameters that are non-sampler descriptor tables
		uint32_t m_SamplerTableBitMap; //one bit is set for root parameters that are sampler descriptor tables
		uint32_t m_DescriptorTableSize[16]; // non-sampler descriptor tables need to know their descriptor count
	};
}

