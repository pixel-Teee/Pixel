#pragma once

#include "DirectXContext.h"

namespace Pixel { 

	class RootParameter
	{
		friend class RootSignature;
	public:

		RootParameter();
		~RootParameter();
		void Clear();

		//------Initialize As Descriptor Range------
		void InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE Type, uint32_t Register, uint32_t Count, 
		D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL, uint32_t Space = 0);
		void InitAsDescriptorTable(uint32_t RangeCount, D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL);
		void SetTableRange(uint32_t RangeIndex, D3D12_DESCRIPTOR_RANGE_TYPE Type, uint32_t Register, uint32_t Count, uint32_t Space = 0);
		//------Initialize As Descriptor Range------

		//------Initialize As Root Constant------
		void InitAsConstants(uint32_t Register, uint32_t NumDwords, D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL,
		uint32_t Space = 0);
		//------Initialize As Root Constant------

		//------Initialize As Constant Buffer View------
		void InitAsConstantBuffer(uint32_t Register, D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL, uint32_t Space = 0);
		//------Initialize As Constant Buffer View------

		//------Initialize As Shader Resource View------
		void InitAsBufferSRV(uint32_t Register, D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL, uint32_t Space = 0);
		//------Initialize As Shader Resource View------

		//------Initialize As Unordered Accessed View-------
		void InitAsBufferUAV(uint32_t Register, D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL, uint32_t Space = 0);
		//------Initialize As Unordered Accessed View-------
		const D3D12_ROOT_PARAMETER& operator()(void) const { return m_RootParam; }
	protected:
		
		//root param, can initialize as descriptor table and root parameter and root descriptor
		D3D12_ROOT_PARAMETER m_RootParam;
	};

	class RootSignature
	{
		friend class DescriptorHandleCache;
	public:
		//root parameter's number and static sampler's number
		RootSignature(uint32_t NumRootParams = 0, uint32_t NumStaticSampler = 0);

		void Reset(uint32_t NumRootParams, uint32_t NumStaticSamplers = 0);

		//------access array------
		RootParameter& operator[](size_t EntryIndex);
		const RootParameter& operator[](size_t EntryIndex) const;
		//------access array------

		void InitStaticSampler(uint32_t Register, const D3D12_SAMPLER_DESC& NonStaticSamplerDesc,
			D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL);

		//flags:flag counld input assembly etc.
		void Finalize(const std::wstring& name, D3D12_ROOT_SIGNATURE_FLAGS Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE);

		ID3D12RootSignature* GetNativeSignature() const;
	protected:
		//have already initialized?
		bool m_finalized;

		uint32_t m_NumParameters;

		uint32_t m_NumSamplers;

		//alreay initialized's static samplers
		uint32_t m_NumInitializedStaticSamplers;

		//root parameter array
		std::unique_ptr<RootParameter[]> m_ParamArray;
		//static sampler desc array
		std::unique_ptr<D3D12_STATIC_SAMPLER_DESC[]> m_SamplerArray;

		ID3D12RootSignature* m_Signature;

		uint32_t m_DescriptorTableBitMap; //one bit is set for root parameters that are non-sampler descriptor tables
		uint32_t m_SamplerTableBitMap; //one bit is set for root parameters that are sampler descriptor tables
		uint32_t m_DescriptorTableSize[16]; // non-sampler descriptor tables need to know their descriptor count
	};
}

