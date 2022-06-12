#include "pxpch.h"

#include "DirectXRenderer.h"
#include "Platform/DirectX/PipelineStateObject/DirectXPipelineStateObject.h"
#include "Pixel/Renderer/Buffer.h"
#include "Platform/DirectX/PipelineStateObject/DirectXRootSignature.h"
#include "Pixel/Renderer/Device/Device.h"
#include "Platform/DirectX/Sampler/SamplerManager.h"
#include "Platform/DirectX/State/DirectXBlenderState.h"
#include "Platform/DirectX/State/DirectXRasterState.h"
#include "Platform/DirectX/State/DirectXDepthState.h"

namespace Pixel {

	DirectXRenderer::DirectXRenderer(Ref<Device> pDevice)
	{
		m_pDevice = pDevice;
		
		//------Create Default Pso------
		m_defaultPso = PSO::CreateGraphicsPso(L"ForwardRendererPso");

		Ref<SamplerDesc> defaultSampler = SamplerDesc::Create();;
		//------Create Root Signature------
		m_rootSignature = RootSignature::Create((uint32_t)RootBindings::MaterialSamplers, 1);
		m_rootSignature->InitStaticSampler(0, defaultSampler);
		(*m_rootSignature)[(size_t)RootBindings::MeshConstants].InitAsConstantBuffer(0, ShaderVisibility::ALL);//root descriptor, only need to bind virtual address
		(*m_rootSignature)[(size_t)RootBindings::MaterialConstants].InitAsConstantBuffer(0, ShaderVisibility::ALL);
		(*m_rootSignature)[(size_t)RootBindings::MaterialSRVs].InitAsDescriptorRange(RangeType::SRV, 0, 10, ShaderVisibility::ALL);
		(*m_rootSignature)[(size_t)RootBindings::MaterialSamplers].InitAsDescriptorRange(RangeType::SAMPLER, 0, 10, ShaderVisibility::ALL);
		m_rootSignature->Finalize(L"RootSignature", RootSignatureFlag::AllowInputAssemblerInputLayout, m_pDevice);
		//------Create Root Signature------

		//-----Create Blend State------
		Ref<BlenderState> pBlendState = BlenderState::Create();
		//-----Create Blend State------

		//------Create Raster State------
		Ref<RasterState> pRasterState = RasterState::Create();
		//------Create Raster State------

		//------Create Depth State------
		Ref<DepthState> pDepthState = DepthState::Create();
		//------Create Depth State------

		m_defaultPso->SetBlendState(pBlendState);
		m_defaultPso->SetRasterizerState(pRasterState);
		m_defaultPso->SetDepthState(pDepthState);
		//------Create Default Pso------
	}

	DirectXRenderer::~DirectXRenderer()
	{
		m_PsoArray.clear();
		//clear pipeline state object hash map
		DirectXPSO::DestroyAll();
	}

	//sematics to dx sematics
	std::string SemanticsToDirectXSemantics(Semantics sematics)
	{
		switch (sematics)
		{
		case Semantics::POSITION:
			return "POSITION";
		case Semantics::TEXCOORD:
			return "TEXCOORD";
		case Semantics::NORMAL:
			return "NORMAL";
		case Semantics::TANGENT:
			return "TANGENT";
		case Semantics::BINORMAL:
			return "BINORMAL";
		case Semantics::COLOR:
			return "COLOR";
		case Semantics::BLENDWEIGHT:
			return "BLENDWEIGHT";
		case Semantics::BLENDINDICES:
			return "BLENDINDICES";
		case Semantics::Editor:
			return "EDITOR";
		case Semantics::FLOAT:
			return "FLOAT";
		}
	}

	DXGI_FORMAT ShaderDataTypeToDXGIFormat(ShaderDataType dataType)
	{
		switch (dataType)
		{
		case ShaderDataType::Float:
			return DXGI_FORMAT_R32_FLOAT;
		case ShaderDataType::Float2:
			return DXGI_FORMAT_R32G32_FLOAT;
		case ShaderDataType::Float3:
			return DXGI_FORMAT_R32G32B32_FLOAT;
		case ShaderDataType::Float4:
			return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case ShaderDataType::Int:
			return DXGI_FORMAT_R32_SINT;
		case ShaderDataType::Int2:
			return DXGI_FORMAT_R32G32_SINT;
		case ShaderDataType::Int3:
			return DXGI_FORMAT_R32G32B32_SINT;
		case ShaderDataType::Int4:
			return DXGI_FORMAT_R32G32B32A32_SINT;
		case ShaderDataType::Bool:
			return DXGI_FORMAT_R8_UINT;
		}
	}

	void DirectXRenderer::CreatePso(BufferLayout& layout)
	{
		//may be release
		D3D12_INPUT_ELEMENT_DESC* ElementArray = new D3D12_INPUT_ELEMENT_DESC[layout.GetElements().size()];

		uint32_t i = 0;
		for (auto& buffElement : layout)
		{
			std::string temp = SemanticsToDirectXSemantics(buffElement.m_sematics);
			ElementArray[i].SemanticName = new char[temp.size()];
			memcpy((void*)ElementArray[i].SemanticName, temp.c_str(), temp.size());
			//ElementArray[i].SemanticName = SemanticsToDirectXSemantics(buffElement.m_sematics).c_str();
			ElementArray[i].SemanticIndex = 0;
			ElementArray[i].Format = ShaderDataTypeToDXGIFormat(buffElement.Type);
			ElementArray[i].InputSlot = 0;
			ElementArray[i].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
			ElementArray[i].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			ElementArray[i].InstanceDataStepRate = 0;
		}
		
		//create forward renderer pos
		//copy constructor
		Ref<PSO> pPso = std::make_shared<GraphicsPSO>(*std::static_pointer_cast<GraphicsPSO>(m_defaultPso));
		m_PsoArray.push_back(pPso);
	}		

}