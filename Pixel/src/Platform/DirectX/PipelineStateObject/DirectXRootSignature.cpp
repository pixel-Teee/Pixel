#include "pxpch.h"

#include "DirectXRootSignature.h"
#include "Platform/DirectX/DirectXDevice.h"
#include "Platform/DirectX/Context/DirectXContext.h"
#include "Platform/DirectX/PipelineStateObject/DirectXRootParameter.h"
#include "Platform/DirectX/TypeUtils.h"
#include "Platform/DirectX/Sampler/SamplerManager.h"

namespace Pixel {

	//key:hash code
	//value:root signature
	static std::map<size_t, Microsoft::WRL::ComPtr<ID3D12RootSignature>> s_RootSignatureHashMap;

	DirectXRootSignature::DirectXRootSignature(uint32_t NumRootParams, uint32_t NumStaticSample)
		:m_finalized(false), m_NumParameters(NumRootParams)
	{
		Reset(NumRootParams, NumStaticSample);
	}

	void DirectXRootSignature::Reset(uint32_t NumRootParams, uint32_t NumStaticSamplers)
	{
		//release origional's root parameter's array
		if (NumRootParams > 0)
		{
			m_ParamArray.resize(NumRootParams);
			for (uint32_t i = 0; i < m_ParamArray.size(); ++i)
			{
				m_ParamArray[i] = std::make_shared<DirectXRootParameter>();
			}
		}
		else
			m_ParamArray.clear();

		//the root parameter's number
		m_NumParameters = NumRootParams;

		if (NumStaticSamplers > 0)
			m_SamplerArray.reset(new D3D12_STATIC_SAMPLER_DESC[NumStaticSamplers]);
		else
			m_SamplerArray = nullptr;

		m_NumSamplers = NumStaticSamplers;

		//alreay initialized's static samplers
		m_NumInitializedStaticSamplers = 0;
	}

	RootParameter& DirectXRootSignature::operator[](size_t EntryIndex)
	{
		PX_CORE_ASSERT(EntryIndex < m_NumParameters, "Out of the range!");
		return *m_ParamArray[EntryIndex];
	}

	const RootParameter& DirectXRootSignature::operator[](size_t EntryIndex) const
	{
		PX_CORE_ASSERT(EntryIndex < m_NumParameters, "Out of the range!");
		return *m_ParamArray[EntryIndex];
	}

	void DirectXRootSignature::InitStaticSampler(uint32_t Register, const D3D12_SAMPLER_DESC& NonStaticSamplerDesc, D3D12_SHADER_VISIBILITY Visibility /*= D3D12_SHADER_VISIBILITY_ALL*/)
	{
		PX_CORE_ASSERT(m_NumInitializedStaticSamplers < m_NumSamplers, "Already initialized static samplers out of the limit!");

		//from this index to initialize the static sampler
		D3D12_STATIC_SAMPLER_DESC& StaticSamplerDesc = m_SamplerArray[m_NumInitializedStaticSamplers++];

		StaticSamplerDesc.Filter = NonStaticSamplerDesc.Filter;
		//------address way------
		StaticSamplerDesc.AddressU = NonStaticSamplerDesc.AddressU;
		StaticSamplerDesc.AddressV = NonStaticSamplerDesc.AddressV;
		StaticSamplerDesc.AddressW = NonStaticSamplerDesc.AddressW;
		//------address way------

		StaticSamplerDesc.MipLODBias = NonStaticSamplerDesc.MipLODBias;
		StaticSamplerDesc.MaxAnisotropy = NonStaticSamplerDesc.MaxAnisotropy;
		StaticSamplerDesc.ComparisonFunc = NonStaticSamplerDesc.ComparisonFunc;
		StaticSamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
		StaticSamplerDesc.MinLOD = NonStaticSamplerDesc.MinLOD;
		StaticSamplerDesc.MaxLOD = NonStaticSamplerDesc.MaxLOD;

		//Shader Start Register
		StaticSamplerDesc.ShaderRegister = Register;
		StaticSamplerDesc.RegisterSpace = 0;
		StaticSamplerDesc.ShaderVisibility = Visibility;

		if (StaticSamplerDesc.AddressU == D3D12_TEXTURE_ADDRESS_MODE_BORDER ||
			StaticSamplerDesc.AddressV == D3D12_TEXTURE_ADDRESS_MODE_BORDER ||
			StaticSamplerDesc.AddressW == D3D12_TEXTURE_ADDRESS_MODE_BORDER)
		{
			PX_CORE_ASSERT(
				//transparent black
				NonStaticSamplerDesc.BorderColor[0] == 0.0f &&
				NonStaticSamplerDesc.BorderColor[1] == 0.0f &&
				NonStaticSamplerDesc.BorderColor[2] == 0.0f &&
				NonStaticSamplerDesc.BorderColor[3] == 0.0f ||
				//opaque black
				NonStaticSamplerDesc.BorderColor[0] == 0.0f &&
				NonStaticSamplerDesc.BorderColor[1] == 0.0f &&
				NonStaticSamplerDesc.BorderColor[2] == 0.0f &&
				NonStaticSamplerDesc.BorderColor[3] == 1.0f ||
				//opaque white
				NonStaticSamplerDesc.BorderColor[0] == 1.0f &&
				NonStaticSamplerDesc.BorderColor[1] == 1.0f &&
				NonStaticSamplerDesc.BorderColor[2] == 1.0f &&
				NonStaticSamplerDesc.BorderColor[3] == 1.0f
				, "Sampler border color doest not match the static sampler limitations!");

			if (NonStaticSamplerDesc.BorderColor[3] == 1.0f)
			{
				if (NonStaticSamplerDesc.BorderColor[0] == 1.0f)
					StaticSamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
				else
					StaticSamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
			}
			else
				StaticSamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		}
	}

	void DirectXRootSignature::InitStaticSampler(uint32_t Register, Ref<SamplerDesc> NonStaticSamplerDesc, ShaderVisibility Visibility /*= ShaderVisibility::ALL*/)
	{
		PX_CORE_ASSERT(m_NumInitializedStaticSamplers < m_NumSamplers, "Already initialized static samplers out of the limit!");

		//from this index to initialize the static sampler
		D3D12_STATIC_SAMPLER_DESC& StaticSamplerDesc = m_SamplerArray[m_NumInitializedStaticSamplers++];

		D3D12_SAMPLER_DESC& samplerDesc = std::static_pointer_cast<DirectXSamplerDesc>(NonStaticSamplerDesc)->m_SamplerDesc;

		StaticSamplerDesc.Filter = samplerDesc.Filter;
		//------address way------
		StaticSamplerDesc.AddressU = samplerDesc.AddressU;
		StaticSamplerDesc.AddressV = samplerDesc.AddressV;
		StaticSamplerDesc.AddressW = samplerDesc.AddressW;
		//------address way------

		StaticSamplerDesc.MipLODBias = samplerDesc.MipLODBias;
		StaticSamplerDesc.MaxAnisotropy = samplerDesc.MaxAnisotropy;
		StaticSamplerDesc.ComparisonFunc = samplerDesc.ComparisonFunc;
		StaticSamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
		StaticSamplerDesc.MinLOD = samplerDesc.MinLOD;
		StaticSamplerDesc.MaxLOD = samplerDesc.MaxLOD;

		//Shader Start Register
		StaticSamplerDesc.ShaderRegister = Register;
		StaticSamplerDesc.RegisterSpace = 0;
		StaticSamplerDesc.ShaderVisibility = ShaderVisibilityToDirectXShaderVisibility(Visibility);

		if (StaticSamplerDesc.AddressU == D3D12_TEXTURE_ADDRESS_MODE_BORDER ||
			StaticSamplerDesc.AddressV == D3D12_TEXTURE_ADDRESS_MODE_BORDER ||
			StaticSamplerDesc.AddressW == D3D12_TEXTURE_ADDRESS_MODE_BORDER)
		{
			PX_CORE_ASSERT(
				//transparent black
				samplerDesc.BorderColor[0] == 0.0f &&
				samplerDesc.BorderColor[1] == 0.0f &&
				samplerDesc.BorderColor[2] == 0.0f &&
				samplerDesc.BorderColor[3] == 0.0f ||
				//opaque black
				samplerDesc.BorderColor[0] == 0.0f &&
				samplerDesc.BorderColor[1] == 0.0f &&
				samplerDesc.BorderColor[2] == 0.0f &&
				samplerDesc.BorderColor[3] == 1.0f ||
				//opaque white
				samplerDesc.BorderColor[0] == 1.0f &&
				samplerDesc.BorderColor[1] == 1.0f &&
				samplerDesc.BorderColor[2] == 1.0f &&
				samplerDesc.BorderColor[3] == 1.0f
				, "Sampler border color doest not match the static sampler limitations!");

			if (samplerDesc.BorderColor[3] == 1.0f)
			{
				if (samplerDesc.BorderColor[0] == 1.0f)
					StaticSamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
				else
					StaticSamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
			}
			else
				StaticSamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		}
	}

	void DirectXRootSignature::Finalize(const std::wstring& name, RootSignatureFlag Flags)
	{
		if (m_finalized)
			return;

		PX_CORE_ASSERT(m_NumInitializedStaticSamplers == m_NumSamplers, "Static sampler don't havent initialized complete!");

		D3D12_ROOT_SIGNATURE_DESC RootDesc;
		//the root parameter's number
		RootDesc.NumParameters = m_NumParameters;

		std::vector<D3D12_ROOT_PARAMETER> ParameterArray;
		for (uint32_t i = 0; i < m_ParamArray.size(); ++i)
		{
			D3D12_ROOT_PARAMETER Parameter = std::static_pointer_cast<DirectXRootParameter>(m_ParamArray[i])->GetNativeRootParameter();
			ParameterArray.push_back(Parameter);
		}

		if(m_NumParameters > 0)
			RootDesc.pParameters = (const D3D12_ROOT_PARAMETER*)(&ParameterArray[0]);

		//static sampler
		RootDesc.NumStaticSamplers = m_NumSamplers;
		RootDesc.pStaticSamplers = (const D3D12_STATIC_SAMPLER_DESC*)m_SamplerArray.get();
		RootDesc.Flags = RootSignatureFlagToDirectXRootSignatureFlag(Flags);

		m_DescriptorTableBitMap = 0;
		m_SamplerTableBitMap = 0;

		size_t HashCode = Utility::HashState((const uint32_t*)&RootDesc.Flags);
		//hash the pStaticSamplers array
		HashCode = Utility::HashState((const uint32_t*)RootDesc.pStaticSamplers, m_NumSamplers, HashCode);

		for (uint32_t Param = 0; Param < m_NumParameters; ++Param)
		{
			const D3D12_ROOT_PARAMETER& RootParam = RootDesc.pParameters[Param];
			m_DescriptorTableSize[Param] = 0;

			//descriptor table
			if (RootParam.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
			{
				PX_CORE_ASSERT(RootParam.DescriptorTable.pDescriptorRanges != nullptr, "RootSignature's descriptor table need equal to nullptr");

				HashCode = Utility::HashState((const uint32_t*)RootParam.DescriptorTable.pDescriptorRanges,
					RootParam.DescriptorTable.NumDescriptorRanges, HashCode);

				if (RootParam.DescriptorTable.pDescriptorRanges->RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER)
					m_SamplerTableBitMap |= (1 << Param);
				else
					m_DescriptorTableBitMap |= (1 << Param);

				//traverse the range table
				for (uint32_t TableRange = 0; TableRange < RootParam.DescriptorTable.NumDescriptorRanges; ++TableRange)
					m_DescriptorTableSize[Param] += RootParam.DescriptorTable.pDescriptorRanges[TableRange].NumDescriptors;
			}
			else
			{
				HashCode = Utility::HashState((const uint32_t*)&RootParam, 1, HashCode);
			}
		}

		//root signature's reference
		ID3D12RootSignature** RSRef = nullptr;

		//Microsoft::WRL::ComPtr<ID3D12RootSignature> pRootSignature;
		bool firstCompile = false;
		{
			//mutex and lock_guard
			static std::mutex s_HashMapMutex;
			std::lock_guard<std::mutex> CS(s_HashMapMutex);

			auto iter = s_RootSignatureHashMap.find(HashCode);

			if (iter == s_RootSignatureHashMap.end())
			{
				RSRef = s_RootSignatureHashMap[HashCode].GetAddressOf();
				//pRootSignature = s_RootSignatureHashMap[HashCode];
				firstCompile = true;
			}
			else
			{
				RSRef = iter->second.GetAddressOf();
			}
		}

		//compile
		if (firstCompile)
		{
			//Blob:binary byte code
			Microsoft::WRL::ComPtr<ID3DBlob> pOutBlob, pErrorBlob;

			D3D12SerializeRootSignature(&RootDesc, D3D_ROOT_SIGNATURE_VERSION_1,
				pOutBlob.GetAddressOf(), pErrorBlob.GetAddressOf());

			//PIXEL_CORE_INFO("{0}", *(pErrorBlob->GetBufferPointer()));
			if (pErrorBlob != nullptr)
			{
				PIXEL_CORE_INFO("{0}", (char*)(pErrorBlob->GetBufferPointer()));
			}
			
			PX_CORE_ASSERT(std::static_pointer_cast<DirectXDevice>(DirectXDevice::Get())->GetDevice()->CreateRootSignature(1, pOutBlob->GetBufferPointer(), pOutBlob->GetBufferSize(),
				IID_PPV_ARGS(m_pRootSig.GetAddressOf())) >= 0, "Create RootSignature Error!");

			//set rootsignature's name
			m_pRootSig->SetName(name.c_str());

			s_RootSignatureHashMap[HashCode] = m_pRootSig;

			PX_CORE_ASSERT(*RSRef == m_pRootSig.Get(), "Create RootSignature Error!");
		}
		else
		{
			while (*RSRef == nullptr)
				std::this_thread::yield();
			m_pRootSig = s_RootSignatureHashMap[HashCode];
		}

		m_finalized = true;
	}

	ID3D12RootSignature* DirectXRootSignature::GetNativeSignature() const
	{
		return m_pRootSig.Get();
	}

	Microsoft::WRL::ComPtr<ID3D12RootSignature> DirectXRootSignature::GetComPtrSignature()
	{
		return m_pRootSig;
	}

	void DirectXRootSignature::DestroyAll()
	{
		s_RootSignatureHashMap.clear();
	}

}