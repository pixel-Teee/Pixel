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
#include "Platform/DirectX/Texture/DirectXFrameBuffer.h"
#include "Platform/DirectX/PipelineStateObject/DirectXShader.h"
#include "Platform/DirectX/Context/GraphicsContext.h"
#include "Platform/DirectX/Buffer/DirectXColorBuffer.h"
#include "Platform/DirectX/Buffer/DepthBuffer.h"
#include "Pixel/Renderer/EditorCamera.h"

namespace Pixel {

	DirectXRenderer::DirectXRenderer()
	{
		
		//------Create Default Pso------
		m_defaultPso = PSO::CreateGraphicsPso(L"ForwardRendererPso");

		Ref<SamplerDesc> defaultSampler = SamplerDesc::Create();;
		//------Create Root Signature------
		m_rootSignature = RootSignature::Create((uint32_t)RootBindings::NumRootBindings, 1);
		m_rootSignature->InitStaticSampler(0, defaultSampler, ShaderVisibility::Pixel);
		(*m_rootSignature)[(size_t)RootBindings::MeshConstants].InitAsConstantBuffer(0, ShaderVisibility::Vertex);//root descriptor, only need to bind virtual address
		(*m_rootSignature)[(size_t)RootBindings::MaterialConstants].InitAsConstantBuffer(2, ShaderVisibility::Pixel);
		(*m_rootSignature)[(size_t)RootBindings::MaterialSRVs].InitAsDescriptorRange(RangeType::SRV, 0, 10, ShaderVisibility::ALL);
		(*m_rootSignature)[(size_t)RootBindings::MaterialSamplers].InitAsDescriptorRange(RangeType::SAMPLER, 1, 10, ShaderVisibility::Pixel);
		(*m_rootSignature)[(size_t)RootBindings::CommonSRVs].InitAsDescriptorRange(RangeType::SRV, 10, 10, ShaderVisibility::Pixel);
		(*m_rootSignature)[(size_t)RootBindings::CommonCBV].InitAsConstantBuffer(1, ShaderVisibility::ALL);
		(*m_rootSignature)[(size_t)RootBindings::SkinMatrices].InitiAsBufferSRV(20, ShaderVisibility::ALL);
		m_rootSignature->Finalize(L"RootSignature", RootSignatureFlag::AllowInputAssemblerInputLayout);
		//------Create Root Signature------

		//------Create Shader--------	
		m_forwardVs = Shader::Create("assets/shaders/ForwardShading/ForwardShading.hlsl", "VS", "vs_5_0");
		m_forwardPs = Shader::Create("assets/shaders/ForwardShading/ForwardShading.hlsl", "PS", "ps_5_0");
		//------Create Shader--------

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
		m_defaultPso->SetRenderTargetFormat(ImageFormat::PX_FORMAT_R8G8B8A8_UNORM, ImageFormat::PX_FORMAT_D24_UNORM_S8_UINT);
		m_defaultPso->SetPrimitiveTopologyType(PiplinePrimitiveTopology::TRIANGLE);
		
		auto [VsBinary, VsBinarySize] = std::static_pointer_cast<DirectXShader>(m_forwardVs)->GetShaderBinary();
		auto [PsBinary, PsBinarySize] = std::static_pointer_cast<DirectXShader>(m_forwardPs)->GetShaderBinary();
		m_defaultPso->SetVertexShader(VsBinary, VsBinarySize);
		m_defaultPso->SetPixelShader(PsBinary, PsBinarySize);

		m_defaultPso->SetRootSignature(m_rootSignature);
		//------Create Default Pso------
	}

	DirectXRenderer::~DirectXRenderer()
	{
		m_PsoArray.clear();
		//clear pipeline state object hash map
		DirectXPSO::DestroyAll();

		DirectXRootSignature::DestroyAll();
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

	uint32_t DirectXRenderer::CreatePso(BufferLayout& layout)
	{
		//may be release
		D3D12_INPUT_ELEMENT_DESC* ElementArray = new D3D12_INPUT_ELEMENT_DESC[layout.GetElements().size()];

		uint32_t i = 0;
		for (auto& buffElement : layout)
		{
			std::string temp = SemanticsToDirectXSemantics(buffElement.m_sematics);
			ElementArray[i].SemanticName = new char[temp.size() + 1];
			std::string temp2(temp.size() + 1, '\0');
			for (uint32_t j = 0; j < temp.size(); ++j)
				temp2[j] = temp[j];
			memcpy((void*)ElementArray[i].SemanticName, temp2.c_str(), temp2.size());
			//ElementArray[i].SemanticName = SemanticsToDirectXSemantics(buffElement.m_sematics).c_str();
			ElementArray[i].SemanticIndex = 0;
			ElementArray[i].Format = ShaderDataTypeToDXGIFormat(buffElement.Type);
			ElementArray[i].InputSlot = 0;
			ElementArray[i].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
			ElementArray[i].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			ElementArray[i].InstanceDataStepRate = 0;

			++i;
		}
		
		//create forward renderer pos
		//copy constructor
		Ref<PSO> pPso = std::make_shared<GraphicsPSO>(*std::static_pointer_cast<GraphicsPSO>(m_defaultPso));
		std::static_pointer_cast<GraphicsPSO>(pPso)->SetInputLayout(layout.GetElements().size(), ElementArray);
		//initialize the pipeline state object
		pPso->Finalize();
		m_PsoArray.push_back(pPso);

		//return pso index
		return m_PsoArray.size();
	}

	void DirectXRenderer::ForwardRendering(Ref<Context> pGraphicsContext, const EditorCamera& camera, std::vector<TransformComponent>& trans,
		std::vector<StaticMeshComponent>& meshs, std::vector<LightComponent>& lights, std::vector<TransformComponent>& lightTrans, Ref<Framebuffer> pFrameBuffer)
	{
		Ref<DirectXFrameBuffer> pDirectxFrameBuffer = std::static_pointer_cast<DirectXFrameBuffer>(pFrameBuffer);
		PX_CORE_ASSERT(pDirectxFrameBuffer->m_pColorBuffers.size() == 1, "color buffer's size is not equal to 1!");

		Ref<GraphicsContext> pContext = std::static_pointer_cast<GraphicsContext>(pGraphicsContext);
		
		m_globalConstants.gViewProjection = glm::transpose(camera.GetViewProjection());
		m_globalConstants.gView = glm::transpose(camera.GetViewMatrix());
		m_globalConstants.gProjection = glm::transpose(camera.GetProjection());
		m_globalConstants.cameraPosition = camera.GetPosition();
		if (lightTrans.size() >= 1)
		{
			m_globalConstants.LightPosition = lightTrans[0].Translation;
			m_globalConstants.LightColor = lights[0].color;
		}

		pContext->SetRootSignature(*m_rootSignature);
		
		pContext->SetDynamicConstantBufferView((uint32_t)RootBindings::CommonCBV, sizeof(GlobalConstants), &m_globalConstants);

		Ref<DescriptorCpuHandle> rtvHandle = pDirectxFrameBuffer->m_pColorBuffers[0]->GetRTV();
		std::vector<Ref<DescriptorCpuHandle>> rtvHandles;
		rtvHandles.push_back(rtvHandle);

		Ref<DescriptorCpuHandle> dsvHandle = pDirectxFrameBuffer->m_pDepthBuffer->GetDSV();

		pContext->SetRenderTargets(1, rtvHandles, dsvHandle);

		//get the framebuffer and bind
		pContext->TransitionResource(*(pDirectxFrameBuffer->m_pColorBuffers[0]), ResourceStates::RenderTarget);
		pContext->TransitionResource(*(pDirectxFrameBuffer->m_pDepthBuffer), ResourceStates::DepthWrite);

		//float color[4] = { 0.3f, 0.2f, 0.6f, 1.0f };
		//clear
		pContext->ClearColor(*(pDirectxFrameBuffer->m_pColorBuffers[0]));
		pContext->ClearDepth(*(pDirectxFrameBuffer->m_pDepthBuffer));

		//set viewport and scissor
		ViewPort vp;
		vp.TopLeftX = 0.0f;
		vp.TopLeftY = 0.0f;
		vp.Width = pDirectxFrameBuffer->GetSpecification().Width;
		vp.Height = pDirectxFrameBuffer->GetSpecification().Height;
		vp.MaxDepth = 1.0f;
		vp.MinDepth = 0.0f;

		PixelRect scissor;
		scissor.Left = 0;
		scissor.Right = pDirectxFrameBuffer->GetSpecification().Width;
		scissor.Top = 0;
		scissor.Bottom = pDirectxFrameBuffer->GetSpecification().Height;

		pContext->SetViewportAndScissor(vp, scissor);

		//draw
		//call the model's draw function
		pContext->SetPrimitiveTopology(PrimitiveTopology::TRIANGLELIST);
		for (uint32_t i = 0; i < meshs.size(); ++i)
		{
			//set pipeline state object
			meshs[i].mesh.Draw(trans[i].GetTransform(), pGraphicsContext);
		}

		pContext->TransitionResource(*(pDirectxFrameBuffer->m_pColorBuffers[0]), ResourceStates::Common);
		pContext->TransitionResource(*(pDirectxFrameBuffer->m_pDepthBuffer), ResourceStates::Common);
		//pContext->Finish(true);
	}

	Ref<PSO> DirectXRenderer::GetPso(uint32_t psoIndex)
	{
		PX_CORE_ASSERT(psoIndex <= m_PsoArray.size() && psoIndex >= 0, "out of pipeline state object's range!");
		return m_PsoArray[psoIndex - 1];
	}

}