#include "pxpch.h"

#include "DirectXRenderer.h"
#include "Platform/DirectX/PipelineStateObject/DirectXPipelineStateObject.h"
#include "Pixel/Renderer/Buffer.h"
#include "Platform/DirectX/PipelineStateObject/DirectXRootSignature.h"
#include "Platform/DirectX/DirectXDevice.h"
#include "Platform/DirectX/Sampler/SamplerManager.h"
#include "Platform/DirectX/State/DirectXBlenderState.h"
#include "Platform/DirectX/State/DirectXRasterState.h"
#include "Platform/DirectX/State/DirectXDepthState.h"
#include "Platform/DirectX/Texture/DirectXFrameBuffer.h"
#include "Platform/DirectX/PipelineStateObject/DirectXShader.h"
#include "Platform/DirectX/Context/GraphicsContext.h"
#include "Platform/DirectX/Buffer/DirectXColorBuffer.h"
#include "Platform/DirectX/Buffer/DepthBuffer.h"
#include "Platform/DirectX/Buffer/DirectXStructuredBuffer.h"
#include "Pixel/Renderer/EditorCamera.h"
#include "Pixel/Math/Math.h"
#include "Platform/DirectX/Buffer/DirectXReadBackBuffer.h"
#include "Pixel/Renderer/Context/ContextManager.h"
#include "Pixel/Renderer/Descriptor/DescriptorHeap.h"
#include "Pixel/Renderer/Descriptor/DescriptorAllocator.h"

namespace Pixel {

	struct UV
	{
		float x;
		float y;
	};

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

		std::vector<ImageFormat> imageFormats = { ImageFormat::PX_FORMAT_R8G8B8A8_UNORM, ImageFormat::PX_FORMAT_R32_SINT };
		m_defaultPso->SetRenderTargetFormats(2, imageFormats.data(), ImageFormat::PX_FORMAT_D24_UNORM_S8_UINT);
		m_defaultPso->SetPrimitiveTopologyType(PiplinePrimitiveTopology::TRIANGLE);
		
		auto [VsBinary, VsBinarySize] = std::static_pointer_cast<DirectXShader>(m_forwardVs)->GetShaderBinary();
		auto [PsBinary, PsBinarySize] = std::static_pointer_cast<DirectXShader>(m_forwardPs)->GetShaderBinary();
		m_defaultPso->SetVertexShader(VsBinary, VsBinarySize);
		m_defaultPso->SetPixelShader(PsBinary, PsBinarySize);

		m_defaultPso->SetRootSignature(m_rootSignature);
		//------Create Default Pso------

		//------Create Picker PSO------
		m_PickerShader = Shader::Create("assets/shaders/ForwardShading/Picker.hlsl", "CSGetPixels", "cs_5_0");
		auto [CsBinary, CsBinarySize] = std::static_pointer_cast<DirectXShader>(m_PickerShader)->GetShaderBinary();
		m_PickerPSO = CreateRef<ComputePSO>(L"Picker PSO");
		m_PickerPSO->SetComputeShader(CsBinary, CsBinarySize);
		m_PickerRootSignature = RootSignature::Create(4, 1);
		m_PickerRootSignature->InitStaticSampler(0, defaultSampler, ShaderVisibility::ALL);
		(*m_PickerRootSignature)[0].InitAsDescriptorTable({ std::make_tuple(RangeType::SRV, 0, 1)}, ShaderVisibility::ALL);
		(*m_PickerRootSignature)[1].InitiAsBufferSRV(1, ShaderVisibility::ALL);
		(*m_PickerRootSignature)[2].InitAsBufferUAV(0, ShaderVisibility::ALL);
		(*m_PickerRootSignature)[3].InitAsConstantBuffer(0, ShaderVisibility::ALL);
		m_PickerRootSignature->Finalize(L"Picker RootSignature", RootSignatureFlag::AllowInputAssemblerInputLayout);
		m_PickerPSO->SetRootSignature(m_PickerRootSignature);
		m_PickerPSO->Finalize();

		m_ComputeSrvHeap = DescriptorHeap::Create(L"Picker Srv Heap", DescriptorHeapType::CBV_UAV_SRV, 1);
		m_TextureHandle = m_ComputeSrvHeap->Alloc(1);

		//m_ComputeCbvHeap = DescriptorHeap::Create(L"Compute Cbv Heap", DescriptorHeapType::CBV_UAV_SRV, 1);
		//m_ImageWidthHandle = m_ComputeCbvHeap->Alloc(1);
		//------Create Picker PSO------

		static float Quad[20] = {
		-1,  1, 0, 0, 0,//<position, texcoord>
		 1,  1, 0, 1, 0,
		-1, -1, 0, 0, 1,
		 1, -1, 0, 1, 1
		};

		static uint32_t QuadIndex[6] = {0, 1, 2, 1, 3, 2};

		//create vertex buffer
		m_pVertexBuffer = VertexBuffer::Create(Quad, 4, 5 * sizeof(float));
		m_pIndexBuffer = IndexBuffer::Create(QuadIndex, 6);

		//------Create Deferred Shading PipelineState-------
		CreateDefaultDeferredShadingPso();
		//------Create Deferred Shading PipelineState-------

		m_DeferredShadingLightGbufferTextureHeap = DescriptorHeap::Create(L"DeferredShadingLightHeap", DescriptorHeapType::CBV_UAV_SRV, 4);
		m_DeferredShadingLightGbufferTextureHandle = m_DeferredShadingLightGbufferTextureHeap->Alloc(4);
		uint32_t DescriptorSize = Device::Get()->GetDescriptorAllocator((uint32_t)DescriptorHeapType::CBV_UAV_SRV)->GetDescriptorSize();
		m_DeferredShadingLightGbufferTextureHandles.resize(4);

		for (uint32_t i = 0; i < 4; ++i)
		{
			DescriptorHandle secondHandle = (*m_DeferredShadingLightGbufferTextureHandle) + i * DescriptorSize;
			m_DeferredShadingLightGbufferTextureHandles[i] = secondHandle;
		}
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

	uint32_t DirectXRenderer::CreateDeferredLightPso(BufferLayout& layout)
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

		Ref<PSO> pPso = m_DefaultLightShadingPso;
		std::static_pointer_cast<GraphicsPSO>(pPso)->SetInputLayout(layout.GetElements().size(), ElementArray);
		//initialize the pipeline state object
		pPso->Finalize();
		m_PsoArray.push_back(pPso);

		//return pso index
		return m_PsoArray.size();
	}

	uint32_t DirectXRenderer::CreateDeferredPso(BufferLayout& layout)
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
		Ref<PSO> pPso = std::make_shared<GraphicsPSO>(*(std::static_pointer_cast<GraphicsPSO>(m_DefaultGeometryShadingPso)));
		std::static_pointer_cast<GraphicsPSO>(pPso)->SetInputLayout(layout.GetElements().size(), ElementArray);
		//initialize the pipeline state object
		pPso->Finalize();
		m_PsoArray.push_back(pPso);

		//return pso index
		return m_PsoArray.size();
	}

	void DirectXRenderer::ForwardRendering(Ref<Context> pGraphicsContext, const EditorCamera& camera, std::vector<TransformComponent>& trans,
		std::vector<StaticMeshComponent>& meshs, std::vector<LightComponent>& lights, std::vector<TransformComponent>& lightTrans, Ref<Framebuffer> pFrameBuffer, std::vector<int32_t>& entityIds)
	{
		Ref<DirectXFrameBuffer> pDirectxFrameBuffer = std::static_pointer_cast<DirectXFrameBuffer>(pFrameBuffer);
		PX_CORE_ASSERT(pDirectxFrameBuffer->m_pColorBuffers.size() == 2, "color buffer's size is not equal to 2!");

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
		Ref<DescriptorCpuHandle> editorHandle = pDirectxFrameBuffer->m_pColorBuffers[1]->GetRTV();
		std::vector<Ref<DescriptorCpuHandle>> rtvHandles;
		rtvHandles.push_back(rtvHandle);
		rtvHandles.push_back(editorHandle);

		Ref<DescriptorCpuHandle> dsvHandle = pDirectxFrameBuffer->m_pDepthBuffer->GetDSV();

		pContext->SetRenderTargets(2, rtvHandles, dsvHandle);

		//get the framebuffer and bind
		pContext->TransitionResource(*(pDirectxFrameBuffer->m_pColorBuffers[0]), ResourceStates::RenderTarget);
		pContext->TransitionResource(*(pDirectxFrameBuffer->m_pColorBuffers[1]), ResourceStates::RenderTarget);
		pContext->TransitionResource(*(pDirectxFrameBuffer->m_pDepthBuffer), ResourceStates::DepthWrite);

		//float color[4] = { 0.3f, 0.2f, 0.6f, 1.0f };
		//clear
		pContext->ClearColor(*(pDirectxFrameBuffer->m_pColorBuffers[0]));
		pContext->ClearColor(*(pDirectxFrameBuffer->m_pColorBuffers[1]));
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
			meshs[i].mesh.Draw(trans[i].GetTransform(), pGraphicsContext, entityIds[i]);
		}

		pContext->TransitionResource(*(pDirectxFrameBuffer->m_pColorBuffers[0]), ResourceStates::Common);
		pContext->TransitionResource(*(pDirectxFrameBuffer->m_pColorBuffers[1]), ResourceStates::UnorderedAccess);
		pContext->TransitionResource(*(pDirectxFrameBuffer->m_pDepthBuffer), ResourceStates::Common);
		//pContext->Finish(true);

		m_Width = pDirectxFrameBuffer->GetSpecification().Width;
		m_Height = pDirectxFrameBuffer->GetSpecification().Height;

		if (m_lastHeight != m_Height || m_lastWidth != m_Width)
		{
			int widthAndHeight[2] = { m_Width, m_Height };
			m_editorImageWidthHeightBuffer = CreateRef<DirectXGpuBuffer>();
			std::static_pointer_cast<DirectXGpuBuffer>(m_editorImageWidthHeightBuffer)->Create(L"ImageWidthBuffer", 2, sizeof(int32_t), &widthAndHeight);
		}

		m_lastWidth = m_Width;
		m_lastHeight = m_Height;
	}

	void DirectXRenderer::DeferredRendering(Ref<Context> pGraphicsContext, const EditorCamera& camera, 
	std::vector<TransformComponent*> trans, std::vector<StaticMeshComponent*> meshs, std::vector<MaterialComponent*> materials,
	std::vector<LightComponent*> lights, std::vector<TransformComponent*> lightTrans,
	Ref<Framebuffer> pFrameBuffer, Ref<Framebuffer> pLightFrameBuffer, std::vector<int32_t>& entityIds)
	{
		Ref<GraphicsContext> pContext = std::static_pointer_cast<GraphicsContext>(pGraphicsContext);

		pContext->SetRootSignature(*m_pDeferredShadingRootSignature);

		Ref<DirectXFrameBuffer> pDirectxFrameBuffer = std::static_pointer_cast<DirectXFrameBuffer>(pFrameBuffer);
		PX_CORE_ASSERT(pDirectxFrameBuffer->m_pColorBuffers.size() == 5, "color buffer's size is not equal to 5!");

		//------get the cpu descriptor handle------
		std::vector<Ref<DescriptorCpuHandle>> m_CpuHandles;
		for (uint32_t i = 0; i < pDirectxFrameBuffer->m_pColorBuffers.size(); ++i)
		{
			m_CpuHandles.push_back(pDirectxFrameBuffer->m_pColorBuffers[i]->GetRTV());
		}
		//------get the cpu descriptor handle------

		Ref<DescriptorCpuHandle> dsvHandle = pDirectxFrameBuffer->m_pDepthBuffer->GetDSV();
		pGraphicsContext->SetRenderTargets(5, m_CpuHandles, dsvHandle);

		for (uint32_t i = 0; i < pDirectxFrameBuffer->m_pColorBuffers.size(); ++i)
		{
			pGraphicsContext->TransitionResource(*pDirectxFrameBuffer->m_pColorBuffers[i], ResourceStates::RenderTarget);
		}
		pGraphicsContext->TransitionResource(*pDirectxFrameBuffer->m_pDepthBuffer, ResourceStates::DepthWrite);

		for (uint32_t i = 0; i < pDirectxFrameBuffer->m_pColorBuffers.size(); ++i)
		{
			//clear color buffer
			pContext->ClearColor(*(pDirectxFrameBuffer->m_pColorBuffers[i]));
		}
		pContext->ClearDepth(*(pDirectxFrameBuffer->m_pDepthBuffer));

		//set primitive topology
		pGraphicsContext->SetPrimitiveTopology(PrimitiveTopology::TRIANGLELIST);

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

		//bind resources
		glm::mat4x4 gViewProjection = glm::transpose(camera.GetViewProjection());
		pContext->SetDynamicConstantBufferView((uint32_t)RootBindings::CommonCBV, sizeof(glm::mat4x4), glm::value_ptr(gViewProjection));
		
		for (uint32_t i = 0; i < meshs.size(); ++i)
		{
			//draw every mesh
			meshs[i]->mesh.Draw(trans[i]->GetTransform(), pContext, entityIds[i], materials[i]);
		}

		for (uint32_t i = 0; i < pDirectxFrameBuffer->m_pColorBuffers.size() - 1; ++i)
		{
			pGraphicsContext->TransitionResource(*pDirectxFrameBuffer->m_pColorBuffers[i], ResourceStates::Common);
		}
		//editor frame buffer
		pContext->TransitionResource(*(pDirectxFrameBuffer->m_pColorBuffers[4]), ResourceStates::Common);
		pContext->TransitionResource(*(pDirectxFrameBuffer->m_pDepthBuffer), ResourceStates::Common);

		//------second pass:light pass------
		pContext->SetPipelineState(*m_DefaultLightShadingPso);
		pContext->SetRootSignature(*m_pDeferredShadingLightRootSignature);
		//bind render targets
		std::vector<Ref<DescriptorCpuHandle>> m_lightFrameBufferCpuHandles;
		Ref<DirectXFrameBuffer> pLightFrame = std::static_pointer_cast<DirectXFrameBuffer>(pLightFrameBuffer);
		m_lightFrameBufferCpuHandles.push_back(pLightFrame->m_pColorBuffers[0]->GetRTV());
		dsvHandle = pLightFrame->m_pDepthBuffer->GetDSV();
		pGraphicsContext->SetRenderTargets(1, m_lightFrameBufferCpuHandles, dsvHandle);

		//clear buffer
		for (uint32_t i = 0; i < pLightFrame->m_pColorBuffers.size() - 1; ++i)
		{
			pGraphicsContext->TransitionResource(*pLightFrame->m_pColorBuffers[i], ResourceStates::RenderTarget);
		}
		pGraphicsContext->TransitionResource(*pLightFrame->m_pDepthBuffer, ResourceStates::DepthWrite);

		for (uint32_t i = 0; i < pLightFrame->m_pColorBuffers.size() - 1; ++i)
		{
			//clear color buffer
			pContext->ClearColor(*(pLightFrame->m_pColorBuffers[i]));
		}
		pContext->ClearDepth(*(pLightFrame->m_pDepthBuffer));

		//set viewport and scissor
		pContext->SetViewportAndScissor(vp, scissor);

		m_lightPass.CameraPosition = camera.GetPosition();
		
		std::vector<LightComponent*> ReSortedPointLight;
		std::vector<LightComponent*> ReSortedDirectLight;
		std::vector<LightComponent*> ReSortedSpotLight;

		for (uint32_t i = 0; i < lights.size(); ++i)
		{
			//---restored point light---
			if (lights[i]->lightType == LightType::PointLight)
			{
				ReSortedPointLight.push_back(lights[i]);
			}
			else if (lights[i]->lightType == LightType::DirectLight)
			{
				ReSortedDirectLight.push_back(lights[i]);
			}
			//---restored point light---
		}

		m_lightPass.PointLightNumber = ReSortedPointLight.size();
		m_lightPass.DirectLightNumber = ReSortedDirectLight.size();

		for (uint32_t i = 0; i < ReSortedPointLight.size(); ++i)
		{
			m_lightPass.lights[i].Position = (*lightTrans[i]).Translation;
			m_lightPass.lights[i].Color = (*lights[i]).color;
			m_lightPass.lights[i].Radius = (*lights[i]).GetSphereLightVolumeRadius();
		}

		for (uint32_t i = 0; i < ReSortedDirectLight.size(); ++i)
		{
			m_lightPass.lights[i + m_lightPass.PointLightNumber].Direction = (*lightTrans[i]).Rotation;
			m_lightPass.lights[i + m_lightPass.PointLightNumber].Color = (*lights[i]).color;
		}

		pContext->SetDynamicConstantBufferView((uint32_t)RootBindings::CommonCBV, sizeof(LightPass), &m_lightPass);

		//copy descriptor handle
		Device::Get()->CopyDescriptorsSimple(1, m_DeferredShadingLightGbufferTextureHandles[0].GetCpuHandle(), pDirectxFrameBuffer->m_pColorBuffers[0]->GetSRV(), DescriptorHeapType::CBV_UAV_SRV);
		Device::Get()->CopyDescriptorsSimple(1, m_DeferredShadingLightGbufferTextureHandles[1].GetCpuHandle(), pDirectxFrameBuffer->m_pColorBuffers[1]->GetSRV(), DescriptorHeapType::CBV_UAV_SRV);
		Device::Get()->CopyDescriptorsSimple(1, m_DeferredShadingLightGbufferTextureHandles[2].GetCpuHandle(), pDirectxFrameBuffer->m_pColorBuffers[2]->GetSRV(), DescriptorHeapType::CBV_UAV_SRV);
		Device::Get()->CopyDescriptorsSimple(1, m_DeferredShadingLightGbufferTextureHandles[3].GetCpuHandle(), pDirectxFrameBuffer->m_pColorBuffers[3]->GetSRV(), DescriptorHeapType::CBV_UAV_SRV);

		//bind texture resources
		pContext->SetDescriptorHeap(DescriptorHeapType::CBV_UAV_SRV, m_DeferredShadingLightGbufferTextureHeap);
		pContext->SetDescriptorTable((uint32_t)RootBindings::MaterialSRVs, m_DeferredShadingLightGbufferTextureHandle->GetGpuHandle());

		//bind vertex buffer and index buffer
		pContext->SetVertexBuffer(0, m_pVertexBuffer->GetVBV());
		pContext->SetIndexBuffer(m_pIndexBuffer->GetIBV());
		pContext->DrawIndexed(m_pIndexBuffer->GetCount());

		for (uint32_t i = 0; i < pLightFrame->m_pColorBuffers.size() - 1; ++i)
		{
			pGraphicsContext->TransitionResource(*pLightFrame->m_pColorBuffers[i], ResourceStates::Common);
		}
		pGraphicsContext->TransitionResource(*pLightFrame->m_pDepthBuffer, ResourceStates::Common);
		//------second pass:light pass------

		m_Width = pDirectxFrameBuffer->GetSpecification().Width;
		m_Height = pDirectxFrameBuffer->GetSpecification().Height;

		if (m_lastHeight != m_Height || m_lastWidth != m_Width)
		{
			int32_t widthAndHeight[2] = { m_Width, m_Height };
			m_editorImageWidthHeightBuffer = CreateRef<DirectXGpuBuffer>();
			std::static_pointer_cast<DirectXGpuBuffer>(m_editorImageWidthHeightBuffer)->Create(L"ImageWidthBuffer", 2, sizeof(int32_t), &widthAndHeight);
		}

		m_lastWidth = m_Width;
		m_lastHeight = m_Height;
	}

	void DirectXRenderer::RenderPickerBuffer(Ref<Context> pComputeContext, Ref<Framebuffer> pFrameBuffer)
	{
		//get the editor buffer
		Ref<DirectXFrameBuffer> pDirectxFrameBuffer = std::static_pointer_cast<DirectXFrameBuffer>(pFrameBuffer);
		PX_CORE_ASSERT(pDirectxFrameBuffer->m_pColorBuffers.size() == 5, "frame color buffer's size error!");

		Ref<DirectXColorBuffer> pColorBuffer = pDirectxFrameBuffer->m_pColorBuffers[4];

		//creat the structed buffer
		m_PickerBuffer = CreateRef<StructuredBuffer>();
		std::static_pointer_cast<StructuredBuffer>(m_PickerBuffer)->Create(L"PickerBuffer", m_Width * m_Height, sizeof(int32_t), nullptr);

		//copy descriptor
		std::static_pointer_cast<DirectXDevice>(Device::Get())->CopyDescriptorsSimple(1, m_TextureHandle->GetCpuHandle(), pColorBuffer->GetSRV(), DescriptorHeapType::CBV_UAV_SRV);

		pComputeContext->SetType(CommandListType::Graphics);

		pComputeContext->SetDescriptorHeap(DescriptorHeapType::CBV_UAV_SRV, m_ComputeSrvHeap);

		//pComputeContext->TransitionResource(*m_UVBuffer, ResourceStates::NonPixelShaderResource, true);
		pComputeContext->TransitionResource(*m_PickerBuffer, ResourceStates::UnorderedAccess, true);
		
		//pComputeContext->SetRootSignature()
		pComputeContext->SetPipelineState(*m_PickerPSO);
		//pComputeContext->SetDynamicSRV(0, )
		pComputeContext->SetRootSignature(*m_PickerRootSignature);
		pComputeContext->SetDescriptorTable(0, m_TextureHandle->GetGpuHandle());
		//pComputeContext->SetBufferSRV(1, *m_UVBuffer);
		pComputeContext->SetBufferUAV(2, *m_PickerBuffer);
		pComputeContext->SetConstantBuffer(3, std::static_pointer_cast<DirectXGpuBuffer>(m_editorImageWidthHeightBuffer)->GetGpuVirtualAddress());
		
		pComputeContext->Dispatch2D(m_Width, m_Height, 32, 32);
	//	pComputeContext->TransitionResource(*pColorBuffer, ResourceStates::Common, true);
	//	pComputeContext->TransitionResource(*pUVBuffer, ResourceStates::Common, true);
		//pComputeContext->TransitionResource(*m_PickerBuffer, ResourceStates::Common, true);
		pComputeContext->SetType(CommandListType::Compute);
		pComputeContext->Finish(true);
	}

	Ref<PSO> DirectXRenderer::GetPso(uint32_t psoIndex)
	{
		PX_CORE_ASSERT(psoIndex <= m_PsoArray.size() && psoIndex >= 0, "out of pipeline state object's range!");
		return m_PsoArray[psoIndex - 1];
	}

	int32_t DirectXRenderer::GetPickerValue(uint32_t x, uint32_t y)
	{
		//creat the read back buffer, and copy the picker buffer to read back buffer
		Ref<ReadBackBuffer> pReadBack = CreateRef<ReadBackBuffer>();
		Ref<StructuredBuffer> pPickBuffer = std::static_pointer_cast<StructuredBuffer>(m_PickerBuffer);
		pReadBack->Create(L"ReadBackBuffer", std::static_pointer_cast<DirectXGpuBuffer>(pPickBuffer)->GetElementCount(), std::static_pointer_cast<DirectXGpuBuffer>(m_PickerBuffer)->GetElementSize());

		Ref<Context> pContext = Device::Get()->GetContextManager()->AllocateContext(CommandListType::Graphics);
		pContext->TransitionResource(*pReadBack, ResourceStates::CopyDest, true);
		pContext->TransitionResource(*pPickBuffer, ResourceStates::CopySource, true);
		pContext->CopyBufferRegion(*pReadBack, 0, *pPickBuffer, 0, std::static_pointer_cast<ReadBackBuffer>(pReadBack)->GetBufferSize());
		pContext->Finish(true);

		int32_t* value = (int32_t*)(pReadBack->Map());

		int32_t returnValue = value[m_Width * y + x];

		pReadBack->UnMap();

		return returnValue;
	}

	void DirectXRenderer::CreateDefaultDeferredShadingPso()
	{
		m_DefaultGeometryShadingPso = PSO::CreateGraphicsPso(L"DeferredShadingGeometryPso");

		Ref<SamplerDesc> samplerDesc = SamplerDesc::Create();

		//------Create RootSignature------
		m_pDeferredShadingRootSignature = RootSignature::Create((uint32_t)RootBindings::NumRootBindings, 1);
		m_pDeferredShadingRootSignature->InitStaticSampler(0, samplerDesc, ShaderVisibility::Pixel);
		(*m_pDeferredShadingRootSignature)[(size_t)RootBindings::MeshConstants].InitAsConstantBuffer(0, ShaderVisibility::Vertex);//root descriptor, only need to bind virtual address
		(*m_pDeferredShadingRootSignature)[(size_t)RootBindings::MaterialConstants].InitAsConstantBuffer(2, ShaderVisibility::Pixel);
		(*m_pDeferredShadingRootSignature)[(size_t)RootBindings::MaterialSRVs].InitAsDescriptorRange(RangeType::SRV, 0, 10, ShaderVisibility::ALL);
		(*m_pDeferredShadingRootSignature)[(size_t)RootBindings::MaterialSamplers].InitAsDescriptorRange(RangeType::SAMPLER, 1, 10, ShaderVisibility::Pixel);
		(*m_pDeferredShadingRootSignature)[(size_t)RootBindings::CommonSRVs].InitAsDescriptorRange(RangeType::SRV, 10, 10, ShaderVisibility::Pixel);
		(*m_pDeferredShadingRootSignature)[(size_t)RootBindings::CommonCBV].InitAsConstantBuffer(1, ShaderVisibility::ALL);
		(*m_pDeferredShadingRootSignature)[(size_t)RootBindings::SkinMatrices].InitiAsBufferSRV(20, ShaderVisibility::ALL);
		m_pDeferredShadingRootSignature->Finalize(L"DeferredShadingRootSignature", RootSignatureFlag::AllowInputAssemblerInputLayout);
		//------Create RootSignature------

		//-----Create Blend State------
		Ref<BlenderState> pBlendState = BlenderState::Create();
		//-----Create Blend State------

		//------Create Raster State------
		Ref<RasterState> pRasterState = RasterState::Create();
		//------Create Raster State------

		//------Create Depth State------
		Ref<DepthState> pDepthState = DepthState::Create();
		//------Create Depth State------

		m_DefaultGeometryShadingPso->SetBlendState(pBlendState);
		m_DefaultGeometryShadingPso->SetRasterizerState(pRasterState);
		m_DefaultGeometryShadingPso->SetDepthState(pDepthState);

		std::vector<ImageFormat> imageFormats = { ImageFormat::PX_FORMAT_R16G16B16A16_FLOAT, ImageFormat::PX_FORMAT_R16G16B16A16_FLOAT, ImageFormat::PX_FORMAT_R8G8B8A8_UNORM,
		ImageFormat::PX_FORMAT_R8G8B8A8_UNORM, ImageFormat::PX_FORMAT_R32_SINT };
		m_DefaultGeometryShadingPso->SetRenderTargetFormats(5, imageFormats.data(), ImageFormat::PX_FORMAT_D24_UNORM_S8_UINT);
		m_DefaultGeometryShadingPso->SetPrimitiveTopologyType(PiplinePrimitiveTopology::TRIANGLE);

		m_GeometryVertexShader = Shader::Create("assets/shaders/DeferredShading/GeometryPass.hlsl", "VS", "vs_5_0");
		m_GeometryPixelShader = Shader::Create("assets/shaders/DeferredShading/GeometryPass.hlsl", "PS", "ps_5_0");

		auto [VsBinary, VsBinarySize] = std::static_pointer_cast<DirectXShader>(m_GeometryVertexShader)->GetShaderBinary();
		auto [PsBinary, PsBinarySize] = std::static_pointer_cast<DirectXShader>(m_GeometryPixelShader)->GetShaderBinary();
		m_DefaultGeometryShadingPso->SetVertexShader(VsBinary, VsBinarySize);
		m_DefaultGeometryShadingPso->SetPixelShader(PsBinary, PsBinarySize);

		m_DefaultGeometryShadingPso->SetRootSignature(m_pDeferredShadingRootSignature);


		//------create light pso------
		m_DefaultLightShadingPso = PSO::CreateGraphicsPso(L"DeferredShadingLightPso");

		//-------create root signature------
		m_pDeferredShadingLightRootSignature = RootSignature::Create((uint32_t)RootBindings::NumRootBindings, 1);
		m_pDeferredShadingLightRootSignature->InitStaticSampler(0, samplerDesc, ShaderVisibility::Pixel);
		(*m_pDeferredShadingLightRootSignature)[(size_t)RootBindings::MeshConstants].InitAsConstantBuffer(0, ShaderVisibility::Vertex);//root descriptor, only need to bind virtual address
		(*m_pDeferredShadingLightRootSignature)[(size_t)RootBindings::MaterialConstants].InitAsConstantBuffer(2, ShaderVisibility::Pixel);
		(*m_pDeferredShadingLightRootSignature)[(size_t)RootBindings::MaterialSRVs].InitAsDescriptorRange(RangeType::SRV, 0, 10, ShaderVisibility::ALL);
		(*m_pDeferredShadingLightRootSignature)[(size_t)RootBindings::MaterialSamplers].InitAsDescriptorRange(RangeType::SAMPLER, 1, 10, ShaderVisibility::Pixel);
		(*m_pDeferredShadingLightRootSignature)[(size_t)RootBindings::CommonSRVs].InitAsDescriptorRange(RangeType::SRV, 10, 10, ShaderVisibility::Pixel);
		(*m_pDeferredShadingLightRootSignature)[(size_t)RootBindings::CommonCBV].InitAsConstantBuffer(1, ShaderVisibility::ALL);
		(*m_pDeferredShadingLightRootSignature)[(size_t)RootBindings::SkinMatrices].InitiAsBufferSRV(20, ShaderVisibility::ALL);
		m_pDeferredShadingLightRootSignature->Finalize(L"DeferredShadingLightRootSignature", RootSignatureFlag::AllowInputAssemblerInputLayout);
		//-------create root signature------

		m_DefaultLightShadingPso->SetBlendState(pBlendState);
		m_DefaultLightShadingPso->SetRasterizerState(pRasterState);
		m_DefaultLightShadingPso->SetDepthState(pDepthState);

		imageFormats = { ImageFormat::PX_FORMAT_R8G8B8A8_UNORM, ImageFormat::PX_FORMAT_R32_SINT };
		m_DefaultLightShadingPso->SetRenderTargetFormats(1, imageFormats.data(), ImageFormat::PX_FORMAT_D24_UNORM_S8_UINT);
		m_DefaultLightShadingPso->SetPrimitiveTopologyType(PiplinePrimitiveTopology::TRIANGLE);

		m_LightVertexShader = Shader::Create("assets/shaders/DeferredShading/LightPass.hlsl", "VS", "vs_5_0");
		m_LightPixelShader = Shader::Create("assets/shaders/DeferredShading/LightPass.hlsl", "PS", "ps_5_0");

		auto [LightVsBinary, LightVsBinarySize] = std::static_pointer_cast<DirectXShader>(m_LightVertexShader)->GetShaderBinary();
		auto [LightPsBinary, LightPsBinarySize] = std::static_pointer_cast<DirectXShader>(m_LightPixelShader)->GetShaderBinary();
		m_DefaultLightShadingPso->SetVertexShader(LightVsBinary, LightVsBinarySize);
		m_DefaultLightShadingPso->SetPixelShader(LightPsBinary, LightPsBinarySize);

		m_DefaultLightShadingPso->SetRootSignature(m_pDeferredShadingLightRootSignature);

		BufferLayout layout = { {ShaderDataType::Float3, "Position", Semantics::POSITION, false},
			{ShaderDataType::Float2, "TexCoord", Semantics::TEXCOORD, false}};

		m_DefaultGeometryShadingPso->SetRootSignature(m_pDeferredShadingLightRootSignature);

		m_DeferredShadingLightPsoIndex = CreateDeferredLightPso(layout);

		//------create light pso------
	}


}