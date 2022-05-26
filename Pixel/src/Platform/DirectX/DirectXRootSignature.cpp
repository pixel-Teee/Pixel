#include "pxpch.h"

#include "DirectXRootSignature.h"
#include "DirectXDevice.h"
#include "DirectXContext.h"
#include "Pixel/Utils/Hash.h"

namespace Pixel {

	//key:hash code
	//value:root signature
	static std::map<size_t, Microsoft::WRL::ComPtr<ID3D12RootSignature>> s_RootSignatureHashMap;

	//------Root Parameter------
	RootParameter::RootParameter()
	{
		m_RootParam.ParameterType = (D3D12_ROOT_PARAMETER_TYPE)0xFFFFFFFF;
	}

	RootParameter::~RootParameter()
	{
		Clear();
	}

	void RootParameter::Clear()
	{
		//delete descriptor table array
 		if (m_RootParam.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
			delete[]m_RootParam.DescriptorTable.pDescriptorRanges;

		m_RootParam.ParameterType = (D3D12_ROOT_PARAMETER_TYPE)0xFFFFFFFF;
	}

	void RootParameter::InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE Type, uint32_t Register, uint32_t Count, D3D12_SHADER_VISIBILITY Visibility, uint32_t Space)
	{
		//allocate descriptor table
		InitAsDescriptorTable(1, Visibility);

		SetTableRange(0, Type, Register, Count, Space);
	}

	void RootParameter::InitAsDescriptorTable(uint32_t RangeCount, D3D12_SHADER_VISIBILITY Visibility)
	{
		m_RootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		m_RootParam.ShaderVisibility = Visibility;
		m_RootParam.DescriptorTable.NumDescriptorRanges = RangeCount;
		m_RootParam.DescriptorTable.pDescriptorRanges = new D3D12_DESCRIPTOR_RANGE[RangeCount];
	}

	void RootParameter::SetTableRange(uint32_t RangeIndex, D3D12_DESCRIPTOR_RANGE_TYPE Type, uint32_t Register, uint32_t Count, uint32_t Space /*= 0*/)
	{
		//get the range index's descriptor table
		D3D12_DESCRIPTOR_RANGE* range = const_cast<D3D12_DESCRIPTOR_RANGE*>(m_RootParam.DescriptorTable.pDescriptorRanges + RangeIndex);

		//set the descriptor table's start register
		range->RangeType = Type;
		range->NumDescriptors = Count;
		range->BaseShaderRegister = Register;
		range->RegisterSpace = Space;
		range->OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	}

	void RootParameter::InitAsConstants(uint32_t Register, uint32_t NumDwords, D3D12_SHADER_VISIBILITY Visibility, uint32_t Space)
	{
		//32bit:double words
		m_RootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
		m_RootParam.ShaderVisibility = Visibility;
		m_RootParam.Constants.Num32BitValues = NumDwords;
		m_RootParam.Constants.ShaderRegister = Register;
		m_RootParam.Constants.RegisterSpace = Space;
	}

	void RootParameter::InitAsConstantBuffer(uint32_t Register, D3D12_SHADER_VISIBILITY Visibility, uint32_t Space)
	{
		m_RootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		m_RootParam.ShaderVisibility = Visibility;
		m_RootParam.Descriptor.ShaderRegister = Register;
		m_RootParam.Descriptor.RegisterSpace = Space;
	}

	void RootParameter::InitAsBufferSRV(uint32_t Register, D3D12_SHADER_VISIBILITY Visibility, uint32_t Space)
	{
		m_RootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
		m_RootParam.ShaderVisibility = Visibility;
		m_RootParam.Descriptor.ShaderRegister = Register;
		m_RootParam.Descriptor.RegisterSpace = Space;
	}

	void RootParameter::InitAsBufferUAV(uint32_t Register, D3D12_SHADER_VISIBILITY Visibility, uint32_t Space)
	{
		m_RootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
		m_RootParam.ShaderVisibility = Visibility;
		m_RootParam.Descriptor.ShaderRegister = Register;
		m_RootParam.Descriptor.RegisterSpace = Space;
	}
	//------Root Parameter------


	RootSignature::RootSignature(uint32_t NumRootParams, uint32_t NumStaticSample)
		:m_finalized(false), m_NumParameters(NumRootParams)
	{
		Reset(NumRootParams, NumStaticSample);
	}

	void RootSignature::Reset(uint32_t NumRootParams, uint32_t NumStaticSamplers)
	{
		//release origional's root parameter's array
		if (NumRootParams > 0)
			m_ParamArray.reset(new RootParameter[NumRootParams]);
		else
			m_ParamArray = nullptr;

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

	Pixel::RootParameter& RootSignature::operator[](size_t EntryIndex)
	{
		PX_CORE_ASSERT(EntryIndex < m_NumParameters, "Out of the range!");
		return m_ParamArray.get()[EntryIndex];
	}

	const Pixel::RootParameter& RootSignature::operator[](size_t EntryIndex) const
	{
		PX_CORE_ASSERT(EntryIndex < m_NumParameters, "Out of the range!");
		return m_ParamArray.get()[EntryIndex];
	}

	void RootSignature::InitStaticSampler(uint32_t Register, const D3D12_SAMPLER_DESC& NonStaticSamplerDesc, D3D12_SHADER_VISIBILITY Visibility /*= D3D12_SHADER_VISIBILITY_ALL*/)
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

	void RootSignature::Finalize(const std::wstring& name, D3D12_ROOT_SIGNATURE_FLAGS Flags)
	{
		if (m_finalized)
			return;

		PX_CORE_ASSERT(m_NumInitializedStaticSamplers == m_NumSamplers, "Static sampler don't havent initialized complete!");

		D3D12_ROOT_SIGNATURE_DESC RootDesc;
		//the root parameter's number
		RootDesc.NumParameters = m_NumParameters;
		RootDesc.pParameters = (const D3D12_ROOT_PARAMETER*)m_ParamArray.get();

		//static sampler
		RootDesc.NumStaticSamplers = m_NumParameters;
		RootDesc.pStaticSamplers = (const D3D12_STATIC_SAMPLER_DESC*)m_SamplerArray.get();
		RootDesc.Flags = Flags;

		size_t HashCode = Utility::HashState(&RootDesc.Flags);
		//hash the pStaticSamplers array
		HashCode = Utility::HashState(RootDesc.pStaticSamplers, m_NumSamplers, HashCode);

		for (uint32_t Param = 0; Param < m_NumParameters; ++Param)
		{
			const D3D12_ROOT_PARAMETER& RootParam = RootDesc.pParameters[Param];

			//descriptor table
			if (RootParam.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
			{
				PX_CORE_ASSERT(RootParam.DescriptorTable.pDescriptorRanges != nullptr, "RootSignature's descriptor table need equal to nullptr");

				HashCode = Utility::HashState(RootParam.DescriptorTable.pDescriptorRanges,
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
				HashCode = Utility::HashState(&RootParam, 1, HashCode);
			}
		}

		//root signature's reference
		ID3D12RootSignature** RSRef = nullptr;

		bool firstCompile = false;
		{
			//mutex and lock_guard
			static std::mutex s_HashMapMutex;
			std::lock_guard<std::mutex> CS(s_HashMapMutex);

			auto iter = s_RootSignatureHashMap.find(HashCode);

			if (iter == s_RootSignatureHashMap.end())
			{
				RSRef = s_RootSignatureHashMap[HashCode].GetAddressOf();
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

			PX_CORE_ASSERT(D3D12SerializeRootSignature(&RootDesc, D3D_ROOT_SIGNATURE_VERSION_1,
				pOutBlob.GetAddressOf(), pErrorBlob.GetAddressOf()) >= 0,
				"Serialize RootSignature Error!");

			PX_CORE_ASSERT(DirectXDevice::Get()->GetDevice()->CreateRootSignature(1, pOutBlob->GetBufferPointer(), pOutBlob->GetBufferSize(),
				IID_PPV_ARGS(&m_Signature)) >= 0, "Create RootSignature Error!");

			//set rootsignature's name
			m_Signature->SetName(name.c_str());

			s_RootSignatureHashMap[HashCode].Attach(m_Signature);

			PX_CORE_ASSERT(*RSRef == m_Signature, "Create RootSignature Error!");
		}
		else
		{
			while (*RSRef == nullptr)
				std::this_thread::yield();
			m_Signature = *RSRef;
		}

		m_finalized = true;
	}

	ID3D12RootSignature* RootSignature::GetNativeSignature() const
	{
		return m_Signature;
	}

}