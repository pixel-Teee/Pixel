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
#include "Platform/DirectX/Texture/DirectXCubeTexture.h"
#include "Platform/DirectX/Texture/DirectXTexture.h"

#include "stb_image.h"

namespace Pixel {

	struct UV
	{
		float x;
		float y;
	};

	struct PrefilterMapPass
	{
		glm::mat4 Projection;
		float Roughness;
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

		m_DeferredShadingLightGbufferTextureHeap = DescriptorHeap::Create(L"DeferredShadingLightHeap", DescriptorHeapType::CBV_UAV_SRV, 7);
		m_DeferredShadingLightGbufferTextureHandle = m_DeferredShadingLightGbufferTextureHeap->Alloc(7);
		uint32_t DescriptorSize = Device::Get()->GetDescriptorAllocator((uint32_t)DescriptorHeapType::CBV_UAV_SRV)->GetDescriptorSize();
		m_DeferredShadingLightGbufferTextureHandles.resize(7);

		for (uint32_t i = 0; i < 7; ++i)
		{
			DescriptorHandle secondHandle = (*m_DeferredShadingLightGbufferTextureHandle) + i * DescriptorSize;
			m_DeferredShadingLightGbufferTextureHandles[i] = secondHandle;
		}

		CreateConvertHDRToCubePipeline();

		m_EquirectangularMap = DescriptorHeap::Create(L"EquirectangularMap", DescriptorHeapType::CBV_UAV_SRV, 1);
		m_EquirectangularDescriptorHandle = m_EquirectangularMap->Alloc(1);

		m_irradianceCubeTextureHeap = DescriptorHeap::Create(L"IrradianceCubeTextureMap", DescriptorHeapType::CBV_UAV_SRV, 1);
		m_irradianceCubeTextureHandle = m_irradianceCubeTextureHeap->Alloc(1);

		std::array<float, 24> vertices = {
			-1.0f, -1.0f, -1.0f,
			-1.0f, +1.0f, -1.0f,
			+1.0f, +1.0f, -1.0f,
			+1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f, +1.0f,
			-1.0f, +1.0f, +1.0f,
			+1.0f, +1.0f, +1.0f,
			+1.0f, -1.0f, +1.0f
		};

		std::array<uint32_t, 36> indices = {
			0, 2, 1,
			0, 3, 2,

			4, 5, 6,
			4, 6, 7,

			4, 1, 5,
			4, 0, 1,

			3, 6, 2,
			3, 7, 6,

			1, 6, 5,
			1, 2, 6,

			4, 3, 0,
			4, 7, 3
		};

		//create cube vertex buffer and index buffer
		m_CubeVertexBuffer = VertexBuffer::Create(vertices.data(), vertices.size() / 3, 3 * sizeof(float));
		m_CubeIndexBuffer = IndexBuffer::Create(indices.data(), indices.size());

		std::array<float, 20> QuadVertices = {
			-1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 1.0f
		};

		std::array<uint32_t, 6> QuadIndices = {
			0, 2, 1, 1, 2, 3
		};

		m_QuadVertexBuffer = VertexBuffer::Create(QuadVertices.data(), QuadVertices.size() / 5, 5 * sizeof(float));
		m_QuadIndexBuffer = IndexBuffer::Create(QuadIndices.data(), QuadIndices.size());

		CreatePrefilterPipeline();
		CreateLutPipeline();
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
		std::vector<TransformComponent*> ReSortedPointLightTrans;
		std::vector<LightComponent*> ReSortedDirectLight;
		std::vector<TransformComponent*> ReSortedDirectLightTrans;
		std::vector<LightComponent*> ReSortedSpotLight;

		for (uint32_t i = 0; i < lights.size(); ++i)
		{
			//---restored point light---
			if (lights[i]->lightType == LightType::PointLight)
			{
				ReSortedPointLight.push_back(lights[i]);
				ReSortedPointLightTrans.push_back(lightTrans[i]);
			}
			else if (lights[i]->lightType == LightType::DirectLight)
			{
				ReSortedDirectLight.push_back(lights[i]);
				ReSortedDirectLightTrans.push_back(lightTrans[i]);
			}
			//---restored point light---
		}

		m_lightPass.PointLightNumber = ReSortedPointLight.size();
		m_lightPass.DirectLightNumber = ReSortedDirectLight.size();

		for (uint32_t i = 0; i < ReSortedPointLight.size(); ++i)
		{
			m_lightPass.lights[i].Position = (*ReSortedPointLightTrans[i]).Translation;
			m_lightPass.lights[i].Color = (*ReSortedPointLight[i]).color;
			m_lightPass.lights[i].Radius = (*ReSortedPointLight[i]).GetSphereLightVolumeRadius();
		}

		for (uint32_t i = 0; i < ReSortedDirectLight.size(); ++i)
		{
			m_lightPass.lights[i + m_lightPass.PointLightNumber].Direction = (*ReSortedDirectLightTrans[i]).Rotation;
			m_lightPass.lights[i + m_lightPass.PointLightNumber].Color = (*ReSortedDirectLight[i]).color;
		}

		pContext->SetDynamicConstantBufferView((uint32_t)RootBindings::CommonCBV, sizeof(LightPass), &m_lightPass);

		Ref<DirectXCubeTexture> m_pIrradianceCubeTexture = std::static_pointer_cast<DirectXCubeTexture>(m_irradianceCubeTexture);

		//copy descriptor handle
		Device::Get()->CopyDescriptorsSimple(1, m_DeferredShadingLightGbufferTextureHandles[0].GetCpuHandle(), pDirectxFrameBuffer->m_pColorBuffers[0]->GetSRV(), DescriptorHeapType::CBV_UAV_SRV);
		Device::Get()->CopyDescriptorsSimple(1, m_DeferredShadingLightGbufferTextureHandles[1].GetCpuHandle(), pDirectxFrameBuffer->m_pColorBuffers[1]->GetSRV(), DescriptorHeapType::CBV_UAV_SRV);
		Device::Get()->CopyDescriptorsSimple(1, m_DeferredShadingLightGbufferTextureHandles[2].GetCpuHandle(), pDirectxFrameBuffer->m_pColorBuffers[2]->GetSRV(), DescriptorHeapType::CBV_UAV_SRV);
		Device::Get()->CopyDescriptorsSimple(1, m_DeferredShadingLightGbufferTextureHandles[3].GetCpuHandle(), pDirectxFrameBuffer->m_pColorBuffers[3]->GetSRV(), DescriptorHeapType::CBV_UAV_SRV);
		Device::Get()->CopyDescriptorsSimple(1, m_DeferredShadingLightGbufferTextureHandles[4].GetCpuHandle(), m_pIrradianceCubeTexture->GetSrvHandle()->GetCpuHandle(), DescriptorHeapType::CBV_UAV_SRV);
		Device::Get()->CopyDescriptorsSimple(1, m_DeferredShadingLightGbufferTextureHandles[5].GetCpuHandle(), m_prefilterMap->GetSrvHandle()->GetCpuHandle(), DescriptorHeapType::CBV_UAV_SRV);
		Device::Get()->CopyDescriptorsSimple(1, m_DeferredShadingLightGbufferTextureHandles[6].GetCpuHandle(), m_LutTexture->GetHandle()->GetCpuHandle(), DescriptorHeapType::CBV_UAV_SRV);
		//bind texture resources
		pContext->SetDescriptorHeap(DescriptorHeapType::CBV_UAV_SRV, m_DeferredShadingLightGbufferTextureHeap);
		pContext->SetDescriptorTable((uint32_t)RootBindings::MaterialSRVs, m_DeferredShadingLightGbufferTextureHandle->GetGpuHandle());

		//bind vertex buffer and index buffer
		pContext->SetVertexBuffer(0, m_pVertexBuffer->GetVBV());
		pContext->SetIndexBuffer(m_pIndexBuffer->GetIBV());
		pContext->DrawIndexed(m_pIndexBuffer->GetCount());
		//------second pass:light pass------

		//------draw sky box------
		pGraphicsContext->SetPipelineState(*m_SkyBoxPso);
		pContext->SetViewportAndScissor(vp, scissor);
		pContext->SetRenderTargets(1, m_lightFrameBufferCpuHandles, dsvHandle);
		pContext->SetRootSignature(*m_SkyBoxRootSignature);

		//Ref<DirectXCubeTexture> m_pIrradianceCubeTexture = std::static_pointer_cast<DirectXCubeTexture>(m_irradianceCubeTexture);

		pContext->TransitionResource(*(m_pIrradianceCubeTexture->m_pCubeTextureResource), ResourceStates::GenericRead);

		Device::Get()->CopyDescriptorsSimple(1, m_irradianceCubeTextureHandle->GetCpuHandle(), m_prefilterMap->GetSrvHandle()->GetCpuHandle(), DescriptorHeapType::CBV_UAV_SRV);

		pContext->SetDescriptorHeap(DescriptorHeapType::CBV_UAV_SRV, m_irradianceCubeTextureHeap);
		pContext->SetDescriptorTable((uint32_t)RootBindings::MaterialSRVs, m_irradianceCubeTextureHandle->GetGpuHandle());

		glm::mat4 NoRotateView = glm::transpose(glm::mat4(glm::mat3(camera.GetViewMatrix())));
		glm::mat4 Projection = glm::transpose(camera.GetProjection());

		glm::mat4 NoRotateViewProjection = NoRotateView * Projection;
		//pContext->SetDynamicConstantBufferView((uint32_t)RootBindings::CommonCBV, sizeof(glm::mat4), glm::value_ptr(glm::transpose(camera.GetViewProjection())));
		pContext->SetDynamicConstantBufferView((uint32_t)RootBindings::CommonCBV, sizeof(glm::mat4), glm::value_ptr(NoRotateViewProjection));
		pContext->SetVertexBuffer(0, m_CubeVertexBuffer->GetVBV());
		pContext->SetIndexBuffer(m_CubeIndexBuffer->GetIBV());
		pContext->DrawIndexed(m_CubeIndexBuffer->GetCount());
		//------draw sky box------

		for (uint32_t i = 0; i < pLightFrame->m_pColorBuffers.size() - 1; ++i)
		{
			pGraphicsContext->TransitionResource(*pLightFrame->m_pColorBuffers[i], ResourceStates::Common);
		}
		pGraphicsContext->TransitionResource(*pLightFrame->m_pDepthBuffer, ResourceStates::Common);

		////------default hdr texture------
		//std::string texturePath = "assets/textures/hdr/brown_photostudio_01_1k.hdr";
		////------default hdr texture------

		//InitializeAndConvertHDRToCubeMap(texturePath);

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

	void DirectXRenderer::InitializeAndConvertHDRToCubeMap(std::string& HdrTexturePath)
	{
		//------Load Hdr Texture------
		int width, height, Components;
		float* data = stbi_loadf(HdrTexturePath.c_str(), &width, &height, &Components, 4);
		m_HDRTexture = Texture2D::Create(16 * width, width, height, ImageFormat::PX_FORMAT_R32G32B32A32_FLOAT, data);
		Ref<DirectXTexture> m_pHDRTexture = std::static_pointer_cast<DirectXTexture>(m_HDRTexture);

		stbi_image_free(data);
		//------Load Hdr Texture------

		m_CubeMapTexture = CubeTexture::Create(512, 512, ImageFormat::PX_FORMAT_R8G8B8A8_UNORM);
		Ref<DirectXCubeTexture> m_pCubeMapTexture = std::static_pointer_cast<DirectXCubeTexture>(m_CubeMapTexture);
		//------Covert HDR Texture To CubeTexture------
		Ref<Context> pContext = Device::Get()->GetContextManager()->AllocateContext(CommandListType::Graphics);
		pContext->SetRootSignature(*m_EquirectangularToCubemapRootSignature);
		pContext->SetPipelineState(*m_HDRConvertToCubePso);
		pContext->SetPrimitiveTopology(PrimitiveTopology::TRIANGLELIST);	

		pContext->TransitionResource(*m_pHDRTexture->m_pGpuResource, ResourceStates::GenericRead);

		//create depth
		FramebufferSpecification fbSpec;
		fbSpec.Attachments = { FramebufferTextureFormat::Depth };
		fbSpec.Width = 512;
		fbSpec.Height = 512;
		m_EquirectangularToCubemapFrameBuffer = Framebuffer::Create(fbSpec);
		Ref<DirectXFrameBuffer> pDirectXFrameBuffer = std::static_pointer_cast<DirectXFrameBuffer>(m_EquirectangularToCubemapFrameBuffer);

		//set viewport and scrssior
		ViewPort vp;
		vp.TopLeftX = 0.0f;
		vp.TopLeftY = 0.0f;
		vp.Width = pDirectXFrameBuffer->GetSpecification().Width;
		vp.Height = pDirectXFrameBuffer->GetSpecification().Height;
		vp.MaxDepth = 1.0f;
		vp.MinDepth = 0.0f;

		PixelRect scissor;
		scissor.Left = 0;
		scissor.Right = pDirectXFrameBuffer->GetSpecification().Width;
		scissor.Top = 0;
		scissor.Bottom = pDirectXFrameBuffer->GetSpecification().Height;

		pContext->SetViewportAndScissor(vp, scissor);

		//bind resource
		pContext->SetDescriptorHeap(DescriptorHeapType::CBV_UAV_SRV, m_EquirectangularMap);
		pContext->SetDescriptorTable((uint32_t)RootBindings::MaterialSRVs, m_EquirectangularDescriptorHandle->GetGpuHandle());
		Device::Get()->CopyDescriptorsSimple(1, m_EquirectangularDescriptorHandle->GetCpuHandle(), m_HDRTexture->GetCpuDescriptorHandle(), DescriptorHeapType::CBV_UAV_SRV);

		glm::mat4 captureProjection = glm::perspectiveLH_ZO(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

		glm::mat4 captureViews[] = {
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, 1.0f,  0.0f)),//+x
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, 1.0f,  0.0f)),//-x
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  -1.0f)),//+y
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, 1.0f)),//-y
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, 1.0f,  0.0f)),//+z
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, 1.0f,  0.0f))//-z
		};

		pContext->TransitionResource(*(m_pCubeMapTexture->m_pCubeTextureResource), ResourceStates::RenderTarget);
		pContext->TransitionResource(*(pDirectXFrameBuffer->m_pDepthBuffer), ResourceStates::DepthWrite);

		for (uint32_t i = 0; i < 6; ++i)
		{
			//clear depth
			std::static_pointer_cast<GraphicsContext>(pContext)->ClearDepth(*(pDirectXFrameBuffer->m_pDepthBuffer));
			
			//bind render target
			//render to cubemap
			pContext->SetRenderTarget(m_pCubeMapTexture->GetRtvHandle(0, i)->GetCpuHandle(), pDirectXFrameBuffer->m_pDepthBuffer->GetDSV());

			glm::mat4 ViewProjection = glm::transpose(captureProjection * captureViews[i]);
			pContext->SetDynamicConstantBufferView((uint32_t)RootBindings::CommonCBV, sizeof(glm::mat4), glm::value_ptr(ViewProjection));

			pContext->SetVertexBuffer(0, m_CubeVertexBuffer->GetVBV());
			pContext->SetIndexBuffer(m_CubeIndexBuffer->GetIBV());
			pContext->DrawIndexed(m_CubeIndexBuffer->GetCount());			
		}	

		pContext->TransitionResource(*(m_pCubeMapTexture->m_pCubeTextureResource), ResourceStates::Common);
		pContext->TransitionResource(*pDirectXFrameBuffer->m_pDepthBuffer, ResourceStates::Common);

		//------generate irradiance convolution cubemap------
		m_irradianceCubeTexture = CubeTexture::Create(32, 32, ImageFormat::PX_FORMAT_R8G8B8A8_UNORM);
		Ref<DirectXCubeTexture> m_pIrradianceCubeTexture = std::static_pointer_cast<DirectXCubeTexture>(m_irradianceCubeTexture);
		pContext->SetRootSignature(*m_convolutionRootSignature);
		pContext->SetPipelineState(*m_convolutionPso);
		pContext->SetPrimitiveTopology(PrimitiveTopology::TRIANGLELIST);

		pContext->TransitionResource(*(m_pCubeMapTexture->m_pCubeTextureResource), ResourceStates::GenericRead);
		pContext->TransitionResource(*(m_pIrradianceCubeTexture->m_pCubeTextureResource), ResourceStates::RenderTarget);
		pContext->TransitionResource(*(pDirectXFrameBuffer->m_pDepthBuffer), ResourceStates::DepthWrite);
		pContext->SetViewportAndScissor(vp, scissor);

		Device::Get()->CopyDescriptorsSimple(1, m_SkyBoxHeapTextureHandle->GetCpuHandle(), m_CubeMapTexture->GetSrvHandle()->GetCpuHandle(), DescriptorHeapType::CBV_UAV_SRV);

		pContext->SetDescriptorHeap(DescriptorHeapType::CBV_UAV_SRV, m_SkyBoxHeap);
		pContext->SetDescriptorTable((uint32_t)RootBindings::MaterialSRVs, m_SkyBoxHeapTextureHandle->GetGpuHandle());

		for (uint32_t i = 0; i < 6; ++i)
		{
			//clear depth
			std::static_pointer_cast<GraphicsContext>(pContext)->ClearDepth(*(pDirectXFrameBuffer->m_pDepthBuffer));

			//bind render target
			//render to cubemap
			pContext->SetRenderTarget(m_pIrradianceCubeTexture->GetRtvHandle(0, i)->GetCpuHandle(), pDirectXFrameBuffer->m_pDepthBuffer->GetDSV());

			glm::mat4 ViewProjection = glm::transpose(captureProjection * captureViews[i]);
			pContext->SetDynamicConstantBufferView((uint32_t)RootBindings::CommonCBV, sizeof(glm::mat4), glm::value_ptr(ViewProjection));

			pContext->SetVertexBuffer(0, m_CubeVertexBuffer->GetVBV());
			pContext->SetIndexBuffer(m_CubeIndexBuffer->GetIBV());
			pContext->DrawIndexed(m_CubeIndexBuffer->GetCount());
		}

		//------generate irradiance convolution cubemap------
		pContext->TransitionResource(*(m_pCubeMapTexture->m_pCubeTextureResource), ResourceStates::Common);
		pContext->TransitionResource(*(pDirectXFrameBuffer->m_pDepthBuffer), ResourceStates::Common);

		//------generate prefilterMap cubemap------
		m_prefilterMap = CubeTexture::Create(128, 128, ImageFormat::PX_FORMAT_R8G8B8A8_UNORM, 5);
		
		Ref<DirectXCubeTexture> m_pPrefilterMap = std::static_pointer_cast<DirectXCubeTexture>(m_prefilterMap);
		pContext->TransitionResource(*(m_pCubeMapTexture->m_pCubeTextureResource), ResourceStates::GenericRead);
		pContext->TransitionResource(*(m_pPrefilterMap->m_pCubeTextureResource), ResourceStates::RenderTarget);
		pContext->SetPipelineState(*m_prefilterPso);
		pContext->SetRootSignature(*m_prefilterRootSignature);
		pContext->SetPrimitiveTopology(PrimitiveTopology::TRIANGLELIST);
		
		uint32_t maxMipLevels = 5;
		for (uint32_t mip = 0; mip < 5; ++mip)
		{
			uint32_t mipWidth = 128 * std::pow(0.5f, mip);
			uint32_t mipHeight = 128 * std::pow(0.5, mip);

			ViewPort vp;
			vp.TopLeftX = 0.0f;
			vp.TopLeftY = 0.0f;
			vp.Width = mipWidth;
			vp.Height = mipHeight;
			vp.MaxDepth = 1.0f;
			vp.MinDepth = 0.0f;

			PixelRect scissor;
			scissor.Left = 0;
			scissor.Right = mipWidth;
			scissor.Top = 0;
			scissor.Bottom = mipHeight;

			fbSpec.Width = mipWidth;
			fbSpec.Height = mipHeight;
			
			m_prefilterFrameBuffer[mip] = Framebuffer::Create(fbSpec);
			Ref<DirectXFrameBuffer> pDirectXFrameBuffer = std::static_pointer_cast<DirectXFrameBuffer>(m_prefilterFrameBuffer[mip]);

			pContext->SetViewportAndScissor(vp, scissor);

			float roughness = (float)mip / (float)(maxMipLevels - 1);
			
			//bind resource
			PrefilterMapPass prefilterPass;
			prefilterPass.Roughness = roughness;
			pContext->TransitionResource(*(pDirectXFrameBuffer->m_pDepthBuffer), ResourceStates::DepthWrite);

			for (uint32_t j = 0; j < 6; ++j)//array slice
			{
				pContext->ClearDepth(*(pDirectXFrameBuffer->m_pDepthBuffer));
				pContext->SetRenderTarget(m_prefilterMap->GetRtvHandle(mip, j)->GetCpuHandle(), pDirectXFrameBuffer->m_pDepthBuffer->GetDSV());

				glm::mat4 ViewProjection = glm::transpose(captureProjection * captureViews[j]);
				prefilterPass.Projection = ViewProjection;

				pContext->SetDynamicConstantBufferView((uint32_t)RootBindings::CommonCBV, sizeof(glm::mat4), &prefilterPass);

				pContext->SetVertexBuffer(0, m_CubeVertexBuffer->GetVBV());
				pContext->SetIndexBuffer(m_CubeIndexBuffer->GetIBV());
				pContext->DrawIndexed(m_CubeIndexBuffer->GetCount());
			}
		}

		pContext->TransitionResource(*(m_pCubeMapTexture->m_pCubeTextureResource), ResourceStates::Common);
		pContext->TransitionResource(*(m_pPrefilterMap->m_pCubeTextureResource), ResourceStates::Common);
		//------generate prefilterMap cubemap------

		//------generate lut------
		m_LutTexture = Texture2D::Create(4 * 512, 512, 512, ImageFormat::PX_FORMAT_R16G16_FLOAT, nullptr);
		Ref<DirectXTexture> pLutTexture = std::static_pointer_cast<DirectXTexture>(m_LutTexture);
		pContext->TransitionResource(*(pLutTexture->m_pGpuResource), ResourceStates::RenderTarget);
		
		pContext->SetPipelineState(*m_LutPso);
		pContext->SetRootSignature(*m_LutRootSignature);
		pContext->SetPrimitiveTopology(PrimitiveTopology::TRIANGLELIST);
		fbSpec.Width = 512;
		fbSpec.Height = 512;

		m_LutFrameBuffer = Framebuffer::Create(fbSpec);
		Ref<DirectXFrameBuffer> pLutFrameBuffer = std::static_pointer_cast<DirectXFrameBuffer>(m_LutFrameBuffer);
		pContext->TransitionResource(*(pLutFrameBuffer->m_pDepthBuffer), ResourceStates::DepthWrite);
		//clear color
		pContext->ClearDepth(*(pLutFrameBuffer->m_pDepthBuffer));

		vp.TopLeftX = 0.0f;
		vp.TopLeftY = 0.0f;
		vp.Width = 512;
		vp.Height = 512;
		vp.MaxDepth = 1.0f;
		vp.MinDepth = 0.0f;

		scissor.Left = 0;
		scissor.Right = 512;
		scissor.Top = 0;
		scissor.Bottom = 512;

		pContext->SetViewportAndScissor(vp, scissor);
		pContext->SetRenderTarget(m_LutTexture->GetRtvHandle()->GetCpuHandle(), pLutFrameBuffer->m_pDepthBuffer->GetDSV());

		pContext->SetVertexBuffer(0, m_QuadVertexBuffer->GetVBV());
		pContext->SetIndexBuffer(m_QuadIndexBuffer->GetIBV());
		pContext->DrawIndexed(m_QuadIndexBuffer->GetCount());

		pContext->TransitionResource(*(pLutTexture->m_pGpuResource), ResourceStates::GenericRead);
		//------generate lut------

		pContext->Finish(true);
	}

	Ref<DescriptorCpuHandle> DirectXRenderer::GetHDRDescriptorHandle()
	{
		return m_HDRTexture->GetCpuDescriptorHandle();
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


	void DirectXRenderer::CreateConvertHDRToCubePipeline()
	{
		m_EquirectangularToCubemapVs = Shader::Create("assets/shaders/IBL/EquirectangularToCubemap.hlsl", "VS", "vs_5_0");
		m_EquirectangularToCubemapFs = Shader::Create("assets/shaders/IBL/EquirectangularToCubemap.hlsl", "PS", "ps_5_0");

		auto [VSShaderBinary, VSBinarySize] = std::static_pointer_cast<DirectXShader>(m_EquirectangularToCubemapVs)->GetShaderBinary();
		auto [PSShaderBinary, PSBinarySize] = std::static_pointer_cast<DirectXShader>(m_EquirectangularToCubemapFs)->GetShaderBinary();

		BufferLayout layout = { {ShaderDataType::Float3, "Position", Semantics::POSITION, false} };

		//------layout------
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
		//------layout------
		m_HDRConvertToCubePso = PSO::CreateGraphicsPso(L"HDRConvertToCubePso");

		m_HDRConvertToCubePso->SetVertexShader(VSShaderBinary, VSBinarySize);
		m_HDRConvertToCubePso->SetPixelShader(PSShaderBinary, PSBinarySize);

		Ref<SamplerDesc> samplerDesc = SamplerDesc::Create();

		m_EquirectangularToCubemapRootSignature = RootSignature::Create((uint32_t)RootBindings::NumRootBindings, 1);
		m_EquirectangularToCubemapRootSignature->InitStaticSampler(0, samplerDesc, ShaderVisibility::Pixel);
		(*m_EquirectangularToCubemapRootSignature)[(size_t)RootBindings::MeshConstants].InitAsConstantBuffer(0, ShaderVisibility::Vertex);//root descriptor, only need to bind virtual address
		(*m_EquirectangularToCubemapRootSignature)[(size_t)RootBindings::MaterialConstants].InitAsConstantBuffer(2, ShaderVisibility::Pixel);
		(*m_EquirectangularToCubemapRootSignature)[(size_t)RootBindings::MaterialSRVs].InitAsDescriptorRange(RangeType::SRV, 0, 10, ShaderVisibility::ALL);
		(*m_EquirectangularToCubemapRootSignature)[(size_t)RootBindings::MaterialSamplers].InitAsDescriptorRange(RangeType::SAMPLER, 1, 10, ShaderVisibility::Pixel);
		(*m_EquirectangularToCubemapRootSignature)[(size_t)RootBindings::CommonSRVs].InitAsDescriptorRange(RangeType::SRV, 10, 10, ShaderVisibility::Pixel);
		(*m_EquirectangularToCubemapRootSignature)[(size_t)RootBindings::CommonCBV].InitAsConstantBuffer(1, ShaderVisibility::ALL);
		(*m_EquirectangularToCubemapRootSignature)[(size_t)RootBindings::SkinMatrices].InitiAsBufferSRV(20, ShaderVisibility::ALL);
		m_EquirectangularToCubemapRootSignature->Finalize(L"DeferredShadingLightRootSignature", RootSignatureFlag::AllowInputAssemblerInputLayout);

		m_HDRConvertToCubePso->SetRootSignature(m_EquirectangularToCubemapRootSignature);

		//-----Create Blend State------
		Ref<BlenderState> pBlendState = BlenderState::Create();
		//-----Create Blend State------

		//------Create Raster State------
		Ref<RasterState> pRasterState = RasterState::Create();
		//------Create Raster State------

		//------Create Depth State------
		Ref<DepthState> pDepthState = DepthState::Create();
		//------Create Depth State------

		m_HDRConvertToCubePso->SetBlendState(pBlendState);
		m_HDRConvertToCubePso->SetRasterizerState(pRasterState);
		m_HDRConvertToCubePso->SetDepthState(pDepthState);

		std::vector<ImageFormat> imageFormats = { ImageFormat::PX_FORMAT_R8G8B8A8_UNORM};
		m_HDRConvertToCubePso->SetRenderTargetFormats(1, imageFormats.data(), ImageFormat::PX_FORMAT_D24_UNORM_S8_UINT);
		m_HDRConvertToCubePso->SetPrimitiveTopologyType(PiplinePrimitiveTopology::TRIANGLE);

		std::static_pointer_cast<GraphicsPSO>(m_HDRConvertToCubePso)->SetInputLayout(layout.GetElements().size(), ElementArray);

		//------initialize pso------
		m_HDRConvertToCubePso->Finalize();
		//------initialize pso------

		//------create sky box pso------
		m_SkyBoxVs = Shader::Create("assets/shaders/IBL/SkyBox.hlsl", "VS", "vs_5_0");
		m_SkyBoxPs = Shader::Create("assets/shaders/IBL/SkyBox.hlsl", "PS", "ps_5_0");
		auto [SkyBoxVSShaderBinary, SkyBoxVSBinarySize] = std::static_pointer_cast<DirectXShader>(m_SkyBoxVs)->GetShaderBinary();
		auto [SkyBoxPSShaderBinary, SkyBoxPSBinarySize] = std::static_pointer_cast<DirectXShader>(m_SkyBoxPs)->GetShaderBinary();

		m_SkyBoxRootSignature = RootSignature::Create((uint32_t)RootBindings::NumRootBindings, 1);
		m_SkyBoxRootSignature->InitStaticSampler(0, samplerDesc, ShaderVisibility::Pixel);
		(*m_SkyBoxRootSignature)[(size_t)RootBindings::MeshConstants].InitAsConstantBuffer(0, ShaderVisibility::Vertex);//root descriptor, only need to bind virtual address
		(*m_SkyBoxRootSignature)[(size_t)RootBindings::MaterialConstants].InitAsConstantBuffer(2, ShaderVisibility::Pixel);
		(*m_SkyBoxRootSignature)[(size_t)RootBindings::MaterialSRVs].InitAsDescriptorRange(RangeType::SRV, 0, 10, ShaderVisibility::ALL);
		(*m_SkyBoxRootSignature)[(size_t)RootBindings::MaterialSamplers].InitAsDescriptorRange(RangeType::SAMPLER, 1, 10, ShaderVisibility::Pixel);
		(*m_SkyBoxRootSignature)[(size_t)RootBindings::CommonSRVs].InitAsDescriptorRange(RangeType::SRV, 10, 10, ShaderVisibility::Pixel);
		(*m_SkyBoxRootSignature)[(size_t)RootBindings::CommonCBV].InitAsConstantBuffer(1, ShaderVisibility::ALL);
		(*m_SkyBoxRootSignature)[(size_t)RootBindings::SkinMatrices].InitiAsBufferSRV(20, ShaderVisibility::ALL);
		m_SkyBoxRootSignature->Finalize(L"SkyBoxRootSignature", RootSignatureFlag::AllowInputAssemblerInputLayout);

		m_SkyBoxPso = PSO::CreateGraphicsPso(L"SkyBoxPso");

		//------element array------
		D3D12_INPUT_ELEMENT_DESC* SkyBoxElementArray = new D3D12_INPUT_ELEMENT_DESC[layout.GetElements().size()];

		i = 0;
		for (auto& buffElement : layout)
		{
			std::string temp = SemanticsToDirectXSemantics(buffElement.m_sematics);
			SkyBoxElementArray[i].SemanticName = new char[temp.size() + 1];
			std::string temp2(temp.size() + 1, '\0');
			for (uint32_t j = 0; j < temp.size(); ++j)
				temp2[j] = temp[j];
			memcpy((void*)SkyBoxElementArray[i].SemanticName, temp2.c_str(), temp2.size());
			//ElementArray[i].SemanticName = SemanticsToDirectXSemantics(buffElement.m_sematics).c_str();
			SkyBoxElementArray[i].SemanticIndex = 0;
			SkyBoxElementArray[i].Format = ShaderDataTypeToDXGIFormat(buffElement.Type);
			SkyBoxElementArray[i].InputSlot = 0;
			SkyBoxElementArray[i].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
			SkyBoxElementArray[i].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			SkyBoxElementArray[i].InstanceDataStepRate = 0;

			++i;
		}
		//------element array------
		m_SkyBoxPso->SetRootSignature(m_SkyBoxRootSignature);

		m_SkyBoxPso->SetVertexShader(SkyBoxVSShaderBinary, SkyBoxVSBinarySize);
		m_SkyBoxPso->SetPixelShader(SkyBoxPSShaderBinary, SkyBoxPSBinarySize);

		pDepthState->SetDepthFunc(DepthFunc::LEQUAL);

		m_SkyBoxPso->SetBlendState(pBlendState);
		m_SkyBoxPso->SetRasterizerState(pRasterState);
		m_SkyBoxPso->SetDepthState(pDepthState);

		m_SkyBoxPso->SetRenderTargetFormats(1, imageFormats.data(), ImageFormat::PX_FORMAT_D24_UNORM_S8_UINT);
		m_SkyBoxPso->SetPrimitiveTopologyType(PiplinePrimitiveTopology::TRIANGLE);

		std::static_pointer_cast<GraphicsPSO>(m_SkyBoxPso)->SetInputLayout(layout.GetElements().size(), SkyBoxElementArray);

		m_SkyBoxPso->Finalize();

		//------create sky box texture descriptor heap------
		m_SkyBoxHeap = DescriptorHeap::Create(L"SkyBoxDescriptorHeap", DescriptorHeapType::CBV_UAV_SRV, 1);
		m_SkyBoxHeapTextureHandle = m_SkyBoxHeap->Alloc(1);
		//------create sky box texture descriptor heap------
		//------create sky box pso------

		//------create convolution pso------
		m_convolutionVs = Shader::Create("assets/shaders/IBL/Convolution.hlsl", "VS", "vs_5_0");
		m_convolutionPs = Shader::Create("assets/shaders/IBL/Convolution.hlsl", "PS", "ps_5_0");

		m_convolutionPso = PSO::CreateGraphicsPso(L"ConvolutionPso");
		auto [ConVolutionVSShaderBinary, ConVolutionVSBinarySize] = std::static_pointer_cast<DirectXShader>(m_convolutionVs)->GetShaderBinary();
		auto [ConVolutionPSShaderBinary, ConVolutionPSBinarySize] = std::static_pointer_cast<DirectXShader>(m_convolutionPs)->GetShaderBinary();

		m_convolutionPso->SetVertexShader(ConVolutionVSShaderBinary, ConVolutionVSBinarySize);
		m_convolutionPso->SetPixelShader(ConVolutionPSShaderBinary, ConVolutionPSBinarySize);

		m_convolutionRootSignature = RootSignature::Create((uint32_t)RootBindings::NumRootBindings, 1);
		m_convolutionRootSignature->InitStaticSampler(0, samplerDesc, ShaderVisibility::Pixel);
		(*m_convolutionRootSignature)[(size_t)RootBindings::MeshConstants].InitAsConstantBuffer(0, ShaderVisibility::Vertex);//root descriptor, only need to bind virtual address
		(*m_convolutionRootSignature)[(size_t)RootBindings::MaterialConstants].InitAsConstantBuffer(2, ShaderVisibility::Pixel);
		(*m_convolutionRootSignature)[(size_t)RootBindings::MaterialSRVs].InitAsDescriptorRange(RangeType::SRV, 0, 10, ShaderVisibility::ALL);
		(*m_convolutionRootSignature)[(size_t)RootBindings::MaterialSamplers].InitAsDescriptorRange(RangeType::SAMPLER, 1, 10, ShaderVisibility::Pixel);
		(*m_convolutionRootSignature)[(size_t)RootBindings::CommonSRVs].InitAsDescriptorRange(RangeType::SRV, 10, 10, ShaderVisibility::Pixel);
		(*m_convolutionRootSignature)[(size_t)RootBindings::CommonCBV].InitAsConstantBuffer(1, ShaderVisibility::ALL);
		(*m_convolutionRootSignature)[(size_t)RootBindings::SkinMatrices].InitiAsBufferSRV(20, ShaderVisibility::ALL);
		m_convolutionRootSignature->Finalize(L"ConvolutionRootSignature", RootSignatureFlag::AllowInputAssemblerInputLayout);

		m_convolutionPso->SetRootSignature(m_convolutionRootSignature);

		pDepthState->SetDepthFunc(DepthFunc::LESS);

		m_convolutionPso->SetBlendState(pBlendState);
		m_convolutionPso->SetDepthState(pDepthState);
		m_convolutionPso->SetRasterizerState(pRasterState);

		m_convolutionPso->SetRenderTargetFormats(1, imageFormats.data(), ImageFormat::PX_FORMAT_D24_UNORM_S8_UINT);
		m_convolutionPso->SetPrimitiveTopologyType(PiplinePrimitiveTopology::TRIANGLE);

		layout = { {ShaderDataType::Float3, "Position", Semantics::POSITION, false}, {ShaderDataType::Float3, "Normal", Semantics::NORMAL, false}};

		D3D12_INPUT_ELEMENT_DESC* ConvolutionElementArray = new D3D12_INPUT_ELEMENT_DESC[layout.GetElements().size()];

		i = 0;
		for (auto& buffElement : layout)
		{
			std::string temp = SemanticsToDirectXSemantics(buffElement.m_sematics);
			ConvolutionElementArray[i].SemanticName = new char[temp.size() + 1];
			std::string temp2(temp.size() + 1, '\0');
			for (uint32_t j = 0; j < temp.size(); ++j)
				temp2[j] = temp[j];
			memcpy((void*)ConvolutionElementArray[i].SemanticName, temp2.c_str(), temp2.size());
			//ElementArray[i].SemanticName = SemanticsToDirectXSemantics(buffElement.m_sematics).c_str();
			ConvolutionElementArray[i].SemanticIndex = 0;
			ConvolutionElementArray[i].Format = ShaderDataTypeToDXGIFormat(buffElement.Type);
			ConvolutionElementArray[i].InputSlot = 0;
			ConvolutionElementArray[i].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
			ConvolutionElementArray[i].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			ConvolutionElementArray[i].InstanceDataStepRate = 0;

			++i;
		}

		std::static_pointer_cast<GraphicsPSO>(m_convolutionPso)->SetInputLayout(layout.GetElements().size(), ConvolutionElementArray);

		m_convolutionPso->Finalize();

		//------create convolution pso------
	}

	void DirectXRenderer::CreatePrefilterPipeline()
	{
		m_prefilterVs = Shader::Create("assets/shaders/IBL/PrefilterMap.hlsl", "VS", "vs_5_0");
		m_prefilterPs = Shader::Create("assets/shaders/IBL/PrefilterMap.hlsl", "PS", "ps_5_0");

		m_prefilterPso = PSO::CreateGraphicsPso(L"PrefilterPso");
		auto [PrefilterMapVSShaderBinary, PrefilterMapVSBinarySize] = std::static_pointer_cast<DirectXShader>(m_prefilterVs)->GetShaderBinary();
		auto [PrefilterMapPSShaderBinary, PrefilterMapPSBinarySize] = std::static_pointer_cast<DirectXShader>(m_prefilterPs)->GetShaderBinary();

		m_prefilterPso->SetVertexShader(PrefilterMapVSShaderBinary, PrefilterMapVSBinarySize);
		m_prefilterPso->SetPixelShader(PrefilterMapPSShaderBinary, PrefilterMapPSBinarySize);

		Ref<SamplerDesc> samplerDesc = SamplerDesc::Create();
		Ref<DepthState> pDepthState = DepthState::Create();
		Ref<BlenderState> pBlendState = BlenderState::Create();
		Ref<RasterState> pRasterState = RasterState::Create();

		m_prefilterRootSignature = RootSignature::Create((uint32_t)RootBindings::NumRootBindings, 1);
		m_prefilterRootSignature->InitStaticSampler(0, samplerDesc, ShaderVisibility::Pixel);
		(*m_prefilterRootSignature)[(size_t)RootBindings::MeshConstants].InitAsConstantBuffer(0, ShaderVisibility::Vertex);//root descriptor, only need to bind virtual address
		(*m_prefilterRootSignature)[(size_t)RootBindings::MaterialConstants].InitAsConstantBuffer(2, ShaderVisibility::Pixel);
		(*m_prefilterRootSignature)[(size_t)RootBindings::MaterialSRVs].InitAsDescriptorRange(RangeType::SRV, 0, 10, ShaderVisibility::ALL);
		(*m_prefilterRootSignature)[(size_t)RootBindings::MaterialSamplers].InitAsDescriptorRange(RangeType::SAMPLER, 1, 10, ShaderVisibility::Pixel);
		(*m_prefilterRootSignature)[(size_t)RootBindings::CommonSRVs].InitAsDescriptorRange(RangeType::SRV, 10, 10, ShaderVisibility::Pixel);
		(*m_prefilterRootSignature)[(size_t)RootBindings::CommonCBV].InitAsConstantBuffer(1, ShaderVisibility::ALL);
		(*m_prefilterRootSignature)[(size_t)RootBindings::SkinMatrices].InitiAsBufferSRV(20, ShaderVisibility::ALL);
		m_prefilterRootSignature->Finalize(L"PrefilterRootSignature", RootSignatureFlag::AllowInputAssemblerInputLayout);

		m_prefilterPso->SetRootSignature(m_convolutionRootSignature);

		pDepthState->SetDepthFunc(DepthFunc::LESS);

		m_prefilterPso->SetBlendState(pBlendState);
		m_prefilterPso->SetDepthState(pDepthState);
		m_prefilterPso->SetRasterizerState(pRasterState);

		std::vector<ImageFormat> imageFormats = { ImageFormat::PX_FORMAT_R8G8B8A8_UNORM };
		m_prefilterPso->SetRenderTargetFormats(1, imageFormats.data(), ImageFormat::PX_FORMAT_D24_UNORM_S8_UINT);
		m_prefilterPso->SetPrimitiveTopologyType(PiplinePrimitiveTopology::TRIANGLE);

		BufferLayout layout = { {ShaderDataType::Float3, "Position", Semantics::POSITION, false}};

		D3D12_INPUT_ELEMENT_DESC* PrefilterElementArray = new D3D12_INPUT_ELEMENT_DESC[layout.GetElements().size()];

		uint32_t i = 0;
		for (auto& buffElement : layout)
		{
			std::string temp = SemanticsToDirectXSemantics(buffElement.m_sematics);
			PrefilterElementArray[i].SemanticName = new char[temp.size() + 1];
			std::string temp2(temp.size() + 1, '\0');
			for (uint32_t j = 0; j < temp.size(); ++j)
				temp2[j] = temp[j];
			memcpy((void*)PrefilterElementArray[i].SemanticName, temp2.c_str(), temp2.size());
			//ElementArray[i].SemanticName = SemanticsToDirectXSemantics(buffElement.m_sematics).c_str();
			PrefilterElementArray[i].SemanticIndex = 0;
			PrefilterElementArray[i].Format = ShaderDataTypeToDXGIFormat(buffElement.Type);
			PrefilterElementArray[i].InputSlot = 0;
			PrefilterElementArray[i].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
			PrefilterElementArray[i].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			PrefilterElementArray[i].InstanceDataStepRate = 0;

			++i;
		}

		std::static_pointer_cast<GraphicsPSO>(m_prefilterPso)->SetInputLayout(layout.GetElements().size(), PrefilterElementArray);

		m_prefilterPso->Finalize();
	}

	void DirectXRenderer::CreateLutPipeline()
	{
		m_LutVs = Shader::Create("assets/shaders/IBL/Lut.hlsl", "VS", "vs_5_0");
		m_LutPs = Shader::Create("assets/shaders/IBL/Lut.hlsl", "PS", "ps_5_0");

		m_LutPso = PSO::CreateGraphicsPso(L"LutPso");
		auto [LutVSShaderBinary, LutVSBinarySize] = std::static_pointer_cast<DirectXShader>(m_LutVs)->GetShaderBinary();
		auto [LutPSShaderBinary, LutPSBinarySize] = std::static_pointer_cast<DirectXShader>(m_LutPs)->GetShaderBinary();

		m_LutPso->SetVertexShader(LutVSShaderBinary, LutVSBinarySize);
		m_LutPso->SetPixelShader(LutPSShaderBinary, LutPSBinarySize);

		Ref<SamplerDesc> samplerDesc = SamplerDesc::Create();
		Ref<DepthState> pDepthState = DepthState::Create();
		Ref<BlenderState> pBlendState = BlenderState::Create();
		Ref<RasterState> pRasterState = RasterState::Create();

		m_LutRootSignature = RootSignature::Create((uint32_t)RootBindings::NumRootBindings, 1);
		m_LutRootSignature->InitStaticSampler(0, samplerDesc, ShaderVisibility::Pixel);
		(*m_LutRootSignature)[(size_t)RootBindings::MeshConstants].InitAsConstantBuffer(0, ShaderVisibility::Vertex);//root descriptor, only need to bind virtual address
		(*m_LutRootSignature)[(size_t)RootBindings::MaterialConstants].InitAsConstantBuffer(2, ShaderVisibility::Pixel);
		(*m_LutRootSignature)[(size_t)RootBindings::MaterialSRVs].InitAsDescriptorRange(RangeType::SRV, 0, 10, ShaderVisibility::ALL);
		(*m_LutRootSignature)[(size_t)RootBindings::MaterialSamplers].InitAsDescriptorRange(RangeType::SAMPLER, 1, 10, ShaderVisibility::Pixel);
		(*m_LutRootSignature)[(size_t)RootBindings::CommonSRVs].InitAsDescriptorRange(RangeType::SRV, 10, 10, ShaderVisibility::Pixel);
		(*m_LutRootSignature)[(size_t)RootBindings::CommonCBV].InitAsConstantBuffer(1, ShaderVisibility::ALL);
		(*m_LutRootSignature)[(size_t)RootBindings::SkinMatrices].InitiAsBufferSRV(20, ShaderVisibility::ALL);
		m_LutRootSignature->Finalize(L"LutRootSignature", RootSignatureFlag::AllowInputAssemblerInputLayout);

		m_LutPso->SetRootSignature(m_LutRootSignature);

		m_LutPso->SetBlendState(pBlendState);
		m_LutPso->SetDepthState(pDepthState);
		m_LutPso->SetRasterizerState(pRasterState);

		std::vector<ImageFormat> imageFormats = { ImageFormat::PX_FORMAT_R16G16_FLOAT };
		m_LutPso->SetRenderTargetFormats(1, imageFormats.data(), ImageFormat::PX_FORMAT_D24_UNORM_S8_UINT);
		m_LutPso->SetPrimitiveTopologyType(PiplinePrimitiveTopology::TRIANGLE);

		BufferLayout layout = { {ShaderDataType::Float3, "Position", Semantics::POSITION, false}, {ShaderDataType::Float2, "TexCoord", Semantics::TEXCOORD, false}};

		D3D12_INPUT_ELEMENT_DESC* LutElementArray = new D3D12_INPUT_ELEMENT_DESC[layout.GetElements().size()];

		uint32_t i = 0;
		for (auto& buffElement : layout)
		{
			std::string temp = SemanticsToDirectXSemantics(buffElement.m_sematics);
			LutElementArray[i].SemanticName = new char[temp.size() + 1];
			std::string temp2(temp.size() + 1, '\0');
			for (uint32_t j = 0; j < temp.size(); ++j)
				temp2[j] = temp[j];
			memcpy((void*)LutElementArray[i].SemanticName, temp2.c_str(), temp2.size());
			//ElementArray[i].SemanticName = SemanticsToDirectXSemantics(buffElement.m_sematics).c_str();
			LutElementArray[i].SemanticIndex = 0;
			LutElementArray[i].Format = ShaderDataTypeToDXGIFormat(buffElement.Type);
			LutElementArray[i].InputSlot = 0;
			LutElementArray[i].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
			LutElementArray[i].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			LutElementArray[i].InstanceDataStepRate = 0;

			++i;
		}

		std::static_pointer_cast<GraphicsPSO>(m_LutPso)->SetInputLayout(layout.GetElements().size(), LutElementArray);

		m_LutPso->Finalize();
	}

}