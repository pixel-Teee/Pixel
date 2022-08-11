#include "pxpch.h"

#include "DirectXRenderer.h"
#include "Platform/DirectX/DirectXDevice.h"

//------pso related------
#include "Platform/DirectX/PipelineStateObject/DirectXPipelineStateObject.h"
#include "Platform/DirectX/PipelineStateObject/DirectXRootSignature.h"
#include "Platform/DirectX/PipelineStateObject/DirectXShader.h"
//------pso related------

//------buffer related------
#include "Pixel/Renderer/Buffer.h"

#include "Platform/DirectX/Buffer/DirectXColorBuffer.h"

#include "Platform/DirectX/Buffer/DirectXStructuredBuffer.h"
#include "Platform/DirectX/Buffer/DirectXReadBackBuffer.h"

#include "Platform/DirectX/Buffer/DepthBuffer.h"
#include "Platform/DirectX/Buffer/ShadowBuffer.h"

#include "Platform/DirectX/Texture/DirectXTexture.h"
#include "Platform/DirectX/Texture/DirectXCubeTexture.h"
#include "Platform/DirectX/Texture/DirectXFrameBuffer.h"
//------buffer related------

//------state releated------
#include "Platform/DirectX/Sampler/SamplerManager.h"
#include "Platform/DirectX/State/DirectXBlenderState.h"
#include "Platform/DirectX/State/DirectXRasterState.h"
#include "Platform/DirectX/State/DirectXDepthState.h"
//------state releated------

//-----context------
#include "Platform/DirectX/Context/GraphicsContext.h"
#include "Pixel/Renderer/Context/ContextManager.h"
//-----context------

//-----others------
#include "Pixel/Renderer/EditorCamera.h"
#include "Pixel/Math/Math.h"
//-----others------

//------descriptor related------
#include "Pixel/Renderer/Descriptor/DescriptorHeap.h"
#include "Pixel/Renderer/Descriptor/DescriptorAllocator.h"
//------descriptor related------

#include "stb_image.h"

#include "Pixel/Scene/Scene.h"

namespace Pixel {

	struct UV
	{
		float x;
		float y;
	};

	//------use for prefilter map------
	struct alignas(256) PrefilterMapPass
	{
		glm::mat4 Projection;
		float Roughness;
	};
	//------use for prefilter map------

	//------use for outline pass------
	struct alignas(256) OutLinePass
	{
		glm::mat4 ViewProjection;
		glm::vec3 camPos;
		float width;
	};
	//------use for outline pass------

	struct alignas(256) TAAPass
	{
		float Width;
		float Height;
		uint32_t frameCount;
	};

	//------use for calculate gauss weights------
	static std::vector<float> CalcGaussWeights(float sigma)
	{
		float twoSigma2 = 2.0f * sigma * sigma;

		int blurRadius = (int)ceil(2.0f * sigma);

		std::vector<float> weights;
		weights.resize(2 * blurRadius + 1);

		float weightSum = 0.0f;

		for (int i = -blurRadius; i <= blurRadius; ++i)
		{
			float x = (float)i;

			weights[i + blurRadius] = expf(-x * x / twoSigma2);

			weightSum += weights[i + blurRadius];
		}

		for (size_t i = 0; i < weights.size(); ++i)
		{
			weights[i] /= weightSum;
		}

		return weights;
	}
	//------use for calculate gauss weights------

	static DXGI_FORMAT ShaderDataTypeToDXGIFormat(ShaderDataType dataType)
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

	//sematics to dx sematics
	static std::string SemanticsToDirectXSemantics(Semantics sematics)
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

	static D3D12_INPUT_ELEMENT_DESC* FromBufferLayoutToCreateDirectXVertexLayout(BufferLayout& layout)
	{
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

		return ElementArray;//life time managed by others
	}

	DirectXRenderer::DirectXRenderer()
	{		
		CreateDefaultForwardRendererPso();//for every model, model will create a forward pso, in terms of forward pso's vertex layout

		CreatePickerPso();//picker renderer

		//------create for quad vertex buffer and quad index buffer------
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
		//------create for quad vertex buffer and quad index buffer------

		//------Create Deferred Shading Geometry and Light PipelineState-------
		CreateDefaultDeferredShadingPso();
		//------Create Deferred Shading Geometry and Light PipelineState-------

		//------Create Deferred Shading Geometry Gbuffer Texture Handle-------
		m_DeferredShadingLightGbufferTextureHeap = DescriptorHeap::Create(L"DeferredShadingLightHeap", DescriptorHeapType::CBV_UAV_SRV, 9);
		m_DeferredShadingLightGbufferTextureHandle = m_DeferredShadingLightGbufferTextureHeap->Alloc(9);
		uint32_t DescriptorSize = Device::Get()->GetDescriptorAllocator((uint32_t)DescriptorHeapType::CBV_UAV_SRV)->GetDescriptorSize();
		m_DeferredShadingLightGbufferTextureHandles.resize(9);

		for (uint32_t i = 0; i < 9; ++i)
		{
			DescriptorHandle secondHandle = (*m_DeferredShadingLightGbufferTextureHandle) + i * DescriptorSize;
			m_DeferredShadingLightGbufferTextureHandles[i] = secondHandle;
		}
		//------Create Deferred Shading Geometry GBuffer Texture Handle------

		//------Create HDR To CubeMap Pipeline------
		CreateConvertHDRToCubePipeline();

		m_EquirectangularMap = DescriptorHeap::Create(L"EquirectangularMap", DescriptorHeapType::CBV_UAV_SRV, 1);
		m_EquirectangularDescriptorHandle = m_EquirectangularMap->Alloc(1);
		//------Create HDR To CubeMap Pipeline------

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

		//------create quad vertex buffer and quad index buffer------
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
		//------create quad vertex buffer and quad index buffer------

		CreatePrefilterPipeline();
		CreateLutPipeline();

		//------render image to back buffer pipeline------
		CreateRenderImageToBackBufferPipeline();
		m_ImageDescriptorHeap = DescriptorHeap::Create(L"ImageDescriptorHeap", DescriptorHeapType::CBV_UAV_SRV, 1);
		m_ImageDescriptorHandle = m_ImageDescriptorHeap->Alloc(1);
		//------render image to back buffer pipeline------

		//------create shadow map and pipeline state object------
		CreateRenderShadowMapPipeline();
		m_ShadowMap = CreateRef<ShadowBuffer>();
		m_ShadowMap->Create(L"ShadowMap", 1024, 1024);
		//------create shadow map and pipeline state object------

		CreateCameraFrustumPipeline();

		//------create additive blending pipeline------
		CreateAdditiveBlendingPipeline();
		m_AdditiveBlendingDescriptorHeap = DescriptorHeap::Create(L"AdditiveBlendingDescriptorHeap", DescriptorHeapType::CBV_UAV_SRV, 2);
		m_AdditiveBlendingDescriptorHandle = m_AdditiveBlendingDescriptorHeap->Alloc(1);
		m_AdditiveBlendingDescriptorHandle2 = m_AdditiveBlendingDescriptorHeap->Alloc(1);
		//------create additive blending pipeline------
	}

	DirectXRenderer::~DirectXRenderer()
	{
		m_PsoArray.clear();
		//clear pipeline state object hash map
		DirectXPSO::DestroyAll();

		//clear root signature
		DirectXRootSignature::DestroyAll();
	}

	void DirectXRenderer::Initialize()
	{
		//------create camera model, for editor debug------
		pCameraModel = CreateRef<Model>("Resources/Icons/Camera.fbx");

		std::string albedoPath = "Resources/Icons/Camera_Albedo.png";
		std::string normalPath = "Resources/Icons/Camera_normal.png";
		std::string roughnessPath = "Resources/Icons/Camera_roughness.png";
		std::string metallicPath = "Resources/Icons/Camera_metallic.png";
		std::string emissivePath = "Resources/Icons/Camera_emissive.png";

		pCameraMaterialComponent = CreateRef<MaterialComponent>(
		albedoPath, normalPath, roughnessPath, metallicPath,
		emissivePath, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 1.0f, 1.0f, false);
		//------create camera model, for editor debug------

		//------create blur texture handle------
		m_BlurTextureUavSrvHeap = DescriptorHeap::Create(L"BlurTexture", DescriptorHeapType::CBV_UAV_SRV, 4);
		m_BlurTextureSrvHandle = m_BlurTextureUavSrvHeap->Alloc(1);
		m_BlurTextureUavHandle = m_BlurTextureUavSrvHeap->Alloc(1);
		m_BlurTexture2SrvHandle = m_BlurTextureUavSrvHeap->Alloc(1);
		m_BlurTexture2UavHandle = m_BlurTextureUavSrvHeap->Alloc(1);

		CreateBlurPipeline();
		//------create blur texture handle------

		//------create point volume pso------
		CreatePointLightVolumePipeline();
		//------create point volume pso------

		//------create point light volume vertex and index------
		std::vector<glm::vec3> pointLightVolumeVertex;
		for (uint32_t i = 0; i < 64; ++i)
		{
			float theta = static_cast<float>(i) / 64 * 360.0f;
			float x = glm::cos(glm::radians(theta));
			float y = glm::sin(glm::radians(theta));
			float z = 0.0f;
			pointLightVolumeVertex.push_back(glm::vec3(x, y, z));
		}
		for (uint32_t i = 0; i < 64; ++i)
		{
			float theta = static_cast<float>(i) / 64 * 360.0f;
			float x = 0.0f;
			float y = glm::sin(glm::radians(theta));
			float z = glm::cos(glm::radians(theta));
			pointLightVolumeVertex.push_back(glm::vec3(x, y, z));
		}
		m_PointLightVolumeVertex = VertexBuffer::Create(glm::value_ptr(*pointLightVolumeVertex.data()), pointLightVolumeVertex.size(), sizeof(glm::vec3));
		
		std::vector<uint32_t> pointLightVolumeIndex;
		for (uint32_t i = 0; i < 64; ++i)
		{
			pointLightVolumeIndex.push_back(i);
			pointLightVolumeIndex.push_back(i + 1);
		}
		pointLightVolumeIndex.back() = 0;
		for (uint32_t i = 64; i < 128; ++i)
		{
			pointLightVolumeIndex.push_back(i);
			pointLightVolumeIndex.push_back(i + 1);
		}
		pointLightVolumeIndex.back() = 64;
		m_PointLightVolumeIndex = IndexBuffer::Create(pointLightVolumeIndex.data(), pointLightVolumeIndex.size());
		//------create point light volume vertex and index------

		CreateOutlinePipeline();

		//------TAA------
		CreateTAAPipeline();
		m_TAATextureDescriptorHeap = DescriptorHeap::Create(L"TAATextureDescriptorHeap", DescriptorHeapType::CBV_UAV_SRV, 4);
		m_TAAFirstTextureDescriptorHandle = m_TAATextureDescriptorHeap->Alloc(4);
		m_TAAFirstTextureDescriptorHandles.resize(4);
		uint32_t DescriptorSize = Device::Get()->GetDescriptorAllocator((uint32_t)DescriptorHeapType::CBV_UAV_SRV)->GetDescriptorSize();
		for (uint32_t i = 0; i < 4; ++i)
		{
			m_TAAFirstTextureDescriptorHandles[i] = (*m_TAAFirstTextureDescriptorHandle) + i * DescriptorSize;
		}
		m_FrameCount = 0;
		//------TAA------
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
			meshs[i].m_Model->Draw(trans[i].GetLocalTransform(), pGraphicsContext, entityIds[i]);
		}

		pContext->TransitionResource(*(pDirectxFrameBuffer->m_pColorBuffers[0]), ResourceStates::Common);
		pContext->TransitionResource(*(pDirectxFrameBuffer->m_pColorBuffers[1]), ResourceStates::UnorderedAccess);
		pContext->TransitionResource(*(pDirectxFrameBuffer->m_pDepthBuffer), ResourceStates::Common);
		//pContext->Finish(true);

		m_Width = pDirectxFrameBuffer->GetSpecification().Width;
		m_Height = pDirectxFrameBuffer->GetSpecification().Height;

		if (m_lastHeight != m_Height || m_lastWidth != m_Width)
		{
			uint32_t widthAndHeight[2] = { m_Width, m_Height };
			m_editorImageWidthHeightBuffer = CreateRef<DirectXGpuBuffer>();
			std::static_pointer_cast<DirectXGpuBuffer>(m_editorImageWidthHeightBuffer)->Create(L"ImageWidthBuffer", 2, sizeof(int32_t), &widthAndHeight);
		}

		m_lastWidth = m_Width;
		m_lastHeight = m_Height;
	}

	void DirectXRenderer::DeferredRendering(Ref<Context> pGraphicsContext, const EditorCamera& camera, 
	std::vector<TransformComponent*> trans, std::vector<StaticMeshComponent*> meshs, std::vector<MaterialComponent*> materials,
	std::vector<LightComponent*> lights, std::vector<TransformComponent*> lightTrans,
	Ref<Framebuffer> pFrameBuffer, Ref<Framebuffer> pLightFrameBuffer, std::vector<int32_t>& entityIds, std::vector<Camera*> pCamera, std::vector<TransformComponent*> cameraTransformant, std::vector<int32_t> cameraEntity, StaticMeshComponent* OutLineMesh, TransformComponent* OutLineMeshTransform, Ref<Scene> scene)
	{
		m_Width = pFrameBuffer->GetSpecification().Width;
		m_Height = pFrameBuffer->GetSpecification().Height;
		if (m_lastHeight != m_Height || m_lastWidth != m_Width)
		{
			uint32_t widthAndHeight[2] = { m_Width, m_Height };
			m_editorImageWidthHeightBuffer = CreateRef<DirectXGpuBuffer>();
			std::static_pointer_cast<DirectXGpuBuffer>(m_editorImageWidthHeightBuffer)->Create(L"ImageWidthBuffer", 2, sizeof(int32_t), &widthAndHeight);

			m_BlurTexture = CreateRef<DirectXColorBuffer>();
			std::static_pointer_cast<DirectXColorBuffer>(m_BlurTexture)->Create(L"BlurTexture", m_Width, m_Height, 0, ImageFormat::PX_FORMAT_R16G16B16A16_FLOAT, nullptr);

			m_BlurTexture2 = CreateRef<DirectXColorBuffer>();
			std::static_pointer_cast<DirectXColorBuffer>(m_BlurTexture2)->Create(L"BlurTexture2", m_Width, m_Height, 0, ImageFormat::PX_FORMAT_R16G16B16A16_FLOAT, nullptr);

			m_PickerBuffer = CreateRef<StructuredBuffer>();
			std::static_pointer_cast<StructuredBuffer>(m_PickerBuffer)->Create(L"PickerBuffer", m_Width * m_Height, sizeof(int32_t), nullptr);

			//------use for TAA------
			m_PreviousScene = CreateRef<DirectXColorBuffer>();
			std::static_pointer_cast<DirectXColorBuffer>(m_PreviousScene)->Create(L"PreviousScene", m_Width, m_Height, 0, ImageFormat::PX_FORMAT_R16G16B16A16_FLOAT, nullptr);

			m_CurrentScene = CreateRef<DirectXColorBuffer>();
			std::static_pointer_cast<DirectXColorBuffer>(m_CurrentScene)->Create(L"CurrentScene", m_Width, m_Height, 0, ImageFormat::PX_FORMAT_R16G16B16A16_FLOAT, nullptr);
			//------use for TAA------
		}

		Ref<GraphicsContext> pContext = std::static_pointer_cast<GraphicsContext>(pGraphicsContext);

		pContext->SetRootSignature(*m_pDeferredShadingRootSignature);

		Ref<DirectXFrameBuffer> pDirectxFrameBuffer = std::static_pointer_cast<DirectXFrameBuffer>(pFrameBuffer);
		PX_CORE_ASSERT(pDirectxFrameBuffer->m_pColorBuffers.size() == 6, "color buffer's size is not equal to 6!");

		//------shadow map------
		LightComponent* mainDirectLight = nullptr;//find the direct light to generate shadow map
		TransformComponent* mainDirectLightComponent = nullptr;
		for (uint32_t i = 0; i < lights.size(); ++i)
		{
			if (lights[i]->GenerateShadowMap)
			{
				mainDirectLight = lights[i];
				mainDirectLightComponent = lightTrans[i];
				break;
			}
		}

		glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);

		if (mainDirectLight != nullptr)
		{
			pContext->SetPipelineState(*m_RenderShadowMapPso);
			pContext->SetRootSignature(*m_RenderShadowMapRootSignature);
			pContext->SetPrimitiveTopology(PrimitiveTopology::TRIANGLELIST);
			m_ShadowMap->BeginRendering(*pContext);

			float nearPlane = 0.01f;
			float farPlane = 500.0f;

			glm::mat4 lightProjection = glm::transpose(glm::orthoLH_ZO(-20.0f, 20.0f, -20.0f, 20.0f, nearPlane, farPlane));
			glm::mat4 lightView = glm::transpose(glm::inverse(mainDirectLightComponent->GetGlobalTransform(scene->GetRegistry())));
			lightSpaceMatrix = lightView * lightProjection;

			pContext->SetDynamicConstantBufferView((uint32_t)RootBindings::CommonCBV, sizeof(glm::mat4), glm::value_ptr(lightSpaceMatrix));

			for (uint32_t i = 0; i < meshs.size(); ++i)
			{
				if(meshs[i]->m_Model != nullptr)
				//draw every mesh
					meshs[i]->m_Model->DrawShadowMap(trans[i]->GetGlobalTransform(scene->GetRegistry()), pContext, entityIds[i]);
			}

			m_ShadowMap->EndRendering(*pContext);
		}
		//------shadow map------

		//------get the cpu descriptor handle------
		std::vector<Ref<DescriptorCpuHandle>> m_CpuHandles;
		for (uint32_t i = 0; i < pDirectxFrameBuffer->m_pColorBuffers.size(); ++i)
		{
			m_CpuHandles.push_back(pDirectxFrameBuffer->m_pColorBuffers[i]->GetRTV());
		}
		//------get the cpu descriptor handle------

		Ref<DescriptorCpuHandle> dsvHandle = pDirectxFrameBuffer->m_pDepthBuffer->GetDSV();
		pGraphicsContext->SetRenderTargets(6, m_CpuHandles, dsvHandle);

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
		m_CbufferGeometryPass.ViewProjection = gViewProjection;
		if (m_IsFirst)
		{
			m_IsFirst = false;
			m_CbufferGeometryPass.previousViewProjection = gViewProjection;
		}
		m_CbufferGeometryPass.frameCount = m_FrameCount;//for taa
		m_CbufferGeometryPass.width = pDirectxFrameBuffer->GetSpecification().Width;
		m_CbufferGeometryPass.height = pDirectxFrameBuffer->GetSpecification().Height;
		++m_FrameCount;
		pContext->SetDynamicConstantBufferView((uint32_t)RootBindings::CommonCBV, sizeof(CbufferGeometryPass), &m_CbufferGeometryPass);
		
		for (uint32_t i = 0; i < meshs.size(); ++i)
		{
			//draw every mesh
			if(meshs[i]->m_Model != nullptr)
				meshs[i]->m_Model->Draw(trans[i]->GetGlobalTransform(scene->GetRegistry()), pContext, entityIds[i], materials[i]);
		}

		//draw runtime camera's model

		for (uint32_t i = 0; i < cameraTransformant.size(); ++i)
		{
			pCameraModel->Draw(cameraTransformant[i]->GetGlobalTransform(scene->GetRegistry()), pContext, (int32_t)cameraEntity[i], pCameraMaterialComponent.get());
		}

		//------draw outline mesh------
		if (OutLineMesh != nullptr && OutLineMesh->m_Model != nullptr)
		{
			pGraphicsContext->SetPipelineState(*m_OutlinePso);
			pGraphicsContext->SetRootSignature(*m_OutlineRootSignature);
			//------outline pass------
			OutLinePass outLinePass;
			glm::mat4x4 gViewProjection = glm::transpose(camera.GetViewProjection());
			outLinePass.ViewProjection = gViewProjection;
			outLinePass.camPos = camera.GetPosition();
			outLinePass.width = m_Width;
			//------outline pass------
			pGraphicsContext->SetDynamicConstantBufferView((uint32_t)RootBindings::CommonCBV, sizeof(OutLinePass), &outLinePass);
			OutLineMesh->m_Model->DrawOutLine(OutLineMeshTransform->GetGlobalTransform(scene->GetRegistry()), pGraphicsContext);
		}
		//------draw outline mesh------

		for (uint32_t i = 0; i < pDirectxFrameBuffer->m_pColorBuffers.size() - 1; ++i)
		{
			pGraphicsContext->TransitionResource(*pDirectxFrameBuffer->m_pColorBuffers[i], ResourceStates::Common);
		}
		//editor frame buffer
		pContext->TransitionResource(*(pDirectxFrameBuffer->m_pColorBuffers[5]), ResourceStates::Common);
		pContext->TransitionResource(*(pDirectxFrameBuffer->m_pDepthBuffer), ResourceStates::Common);

		//------second pass:light pass------
		pContext->SetPipelineState(*m_DefaultLightShadingPso);
		pContext->SetRootSignature(*m_pDeferredShadingLightRootSignature);
		//bind render targets
		std::vector<Ref<DescriptorCpuHandle>> m_lightFrameBufferCpuHandles;
		Ref<DirectXFrameBuffer> pLightFrame = std::static_pointer_cast<DirectXFrameBuffer>(pLightFrameBuffer);
		m_lightFrameBufferCpuHandles.push_back(pLightFrame->m_pColorBuffers[0]->GetRTV());
		m_lightFrameBufferCpuHandles.push_back(pLightFrame->m_pColorBuffers[1]->GetRTV());
		dsvHandle = pLightFrame->m_pDepthBuffer->GetDSV();
		pGraphicsContext->SetRenderTargets(2, m_lightFrameBufferCpuHandles, dsvHandle);

		//clear buffer
		for (uint32_t i = 0; i < pLightFrame->m_pColorBuffers.size(); ++i)
		{
			pGraphicsContext->TransitionResource(*pLightFrame->m_pColorBuffers[i], ResourceStates::RenderTarget);
		}
		pGraphicsContext->TransitionResource(*pLightFrame->m_pDepthBuffer, ResourceStates::DepthWrite);

		for (uint32_t i = 0; i < pLightFrame->m_pColorBuffers.size(); ++i)
		{
			//clear color buffer
			pContext->ClearColor(*(pLightFrame->m_pColorBuffers[i]));
		}
		pContext->ClearDepth(*(pLightFrame->m_pDepthBuffer));

		//set viewport and scissor
		pContext->SetViewportAndScissor(vp, scissor);

		m_lightPass.CameraPosition = camera.GetPosition();
		m_lightPass.receiveAmbientLight = false;//test
		
		std::vector<LightComponent*> ReSortedPointLight;
		std::vector<TransformComponent*> ReSortedPointLightTrans;
		std::vector<LightComponent*> ReSortedDirectLight;
		std::vector<TransformComponent*> ReSortedDirectLightTrans;
		std::vector<LightComponent*> ReSortedSpotLight;
		std::vector<TransformComponent*> ReSortedSpotLightTrans;

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
			else if(lights[i]->lightType == LightType::SpotLight)
			{
				ReSortedSpotLight.push_back(lights[i]);
				ReSortedSpotLightTrans.push_back(lightTrans[i]);
			}
			//---restored point light---
		}

		m_lightPass.PointLightNumber = ReSortedPointLight.size();
		m_lightPass.DirectLightNumber = ReSortedDirectLight.size();
		m_lightPass.SpotLightNumber = ReSortedSpotLight.size();

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
			if (ReSortedDirectLight[i] == mainDirectLight)
			{
				m_lightPass.lights[i + m_lightPass.PointLightNumber].GenerateShadow = 1;
			}
		}

		for(size_t i = 0; i < ReSortedSpotLight.size(); ++i)
		{
			m_lightPass.lights[i + m_lightPass.PointLightNumber + m_lightPass.DirectLightNumber].Position = (*ReSortedSpotLightTrans[i]).Translation;
			m_lightPass.lights[i + m_lightPass.PointLightNumber + m_lightPass.DirectLightNumber].Direction = (*ReSortedSpotLightTrans[i]).Rotation;
			m_lightPass.lights[i + m_lightPass.PointLightNumber + m_lightPass.DirectLightNumber].CutOff = glm::cos(glm::radians(( * ReSortedSpotLight[i]).CutOff));
		}

		if (mainDirectLight != nullptr)
		{
			m_lightPass.LightSpaceMatrix = lightSpaceMatrix;
		}

		pContext->SetDynamicConstantBufferView((uint32_t)RootBindings::CommonCBV, sizeof(LightPass), &m_lightPass);

		Ref<DirectXCubeTexture> m_pIrradianceCubeTexture = std::static_pointer_cast<DirectXCubeTexture>(m_irradianceCubeTexture);

		//copy descriptor handle
		Device::Get()->CopyDescriptorsSimple(1, m_DeferredShadingLightGbufferTextureHandles[0].GetCpuHandle(), pDirectxFrameBuffer->m_pColorBuffers[0]->GetSRV(), DescriptorHeapType::CBV_UAV_SRV);
		Device::Get()->CopyDescriptorsSimple(1, m_DeferredShadingLightGbufferTextureHandles[1].GetCpuHandle(), pDirectxFrameBuffer->m_pColorBuffers[1]->GetSRV(), DescriptorHeapType::CBV_UAV_SRV);
		Device::Get()->CopyDescriptorsSimple(1, m_DeferredShadingLightGbufferTextureHandles[2].GetCpuHandle(), pDirectxFrameBuffer->m_pColorBuffers[2]->GetSRV(), DescriptorHeapType::CBV_UAV_SRV);
		Device::Get()->CopyDescriptorsSimple(1, m_DeferredShadingLightGbufferTextureHandles[3].GetCpuHandle(), pDirectxFrameBuffer->m_pColorBuffers[3]->GetSRV(), DescriptorHeapType::CBV_UAV_SRV);
		Device::Get()->CopyDescriptorsSimple(1, m_DeferredShadingLightGbufferTextureHandles[4].GetCpuHandle(), pDirectxFrameBuffer->m_pColorBuffers[4]->GetSRV(), DescriptorHeapType::CBV_UAV_SRV);
		Device::Get()->CopyDescriptorsSimple(1, m_DeferredShadingLightGbufferTextureHandles[5].GetCpuHandle(), m_pIrradianceCubeTexture->GetSrvHandle()->GetCpuHandle(), DescriptorHeapType::CBV_UAV_SRV);
		Device::Get()->CopyDescriptorsSimple(1, m_DeferredShadingLightGbufferTextureHandles[6].GetCpuHandle(), m_prefilterMap->GetSrvHandle()->GetCpuHandle(), DescriptorHeapType::CBV_UAV_SRV);
		Device::Get()->CopyDescriptorsSimple(1, m_DeferredShadingLightGbufferTextureHandles[7].GetCpuHandle(), m_LutTexture->GetHandle()->GetCpuHandle(), DescriptorHeapType::CBV_UAV_SRV);
		Device::Get()->CopyDescriptorsSimple(1, m_DeferredShadingLightGbufferTextureHandles[8].GetCpuHandle(), m_ShadowMap->GetDepthSRV(), DescriptorHeapType::CBV_UAV_SRV);
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

		Device::Get()->CopyDescriptorsSimple(1, m_irradianceCubeTextureHandle->GetCpuHandle(), m_CubeMapTexture->GetSrvHandle()->GetCpuHandle(), DescriptorHeapType::CBV_UAV_SRV);

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

		//------copy current scene------
		pContext->CopyBuffer(*m_CurrentScene, *(pLightFrame->m_pColorBuffers[0]));
		//------copy current scene------

		//------TAA------
		Ref<DirectXFrameBuffer> pGeometryFrameBuffer = std::static_pointer_cast<DirectXFrameBuffer>(pFrameBuffer);
		Ref<DescriptorCpuHandle> pDsv = pGeometryFrameBuffer->m_pDepthBuffer->GetDepthSRV();
		Ref<DescriptorCpuHandle> pCurrentScene = m_CurrentScene->GetSRV();
		Ref<DescriptorCpuHandle> velocityTexture = pGeometryFrameBuffer->m_pColorBuffers[2]->GetSRV();//velocity texture, get from the geometry framebuffer
		Ref<DescriptorCpuHandle> previousScene = m_PreviousScene->GetSRV();
		Device::Get()->CopyDescriptorsSimple(1, m_TAAFirstTextureDescriptorHandles[0].GetCpuHandle(), pCurrentScene, DescriptorHeapType::CBV_UAV_SRV);
		Device::Get()->CopyDescriptorsSimple(1, m_TAAFirstTextureDescriptorHandles[1].GetCpuHandle(), previousScene, DescriptorHeapType::CBV_UAV_SRV);
		Device::Get()->CopyDescriptorsSimple(1, m_TAAFirstTextureDescriptorHandles[2].GetCpuHandle(), velocityTexture, DescriptorHeapType::CBV_UAV_SRV);
		Device::Get()->CopyDescriptorsSimple(1, m_TAAFirstTextureDescriptorHandles[3].GetCpuHandle(), pDsv, DescriptorHeapType::CBV_UAV_SRV);
		pContext->SetDescriptorHeap(DescriptorHeapType::CBV_UAV_SRV, m_TAATextureDescriptorHeap);
		pContext->SetDescriptorTable((uint32_t)RootBindings::MaterialSRVs, m_TAAFirstTextureDescriptorHandle->GetGpuHandle());
		pContext->SetPipelineState(*m_TAAPSO);
		pContext->SetRootSignature(*m_TAARootSignature);
		TAAPass taaPass;
		taaPass.frameCount = m_FrameCount;
		taaPass.Width = pDirectxFrameBuffer->GetSpecification().Width;
		taaPass.Height = pDirectxFrameBuffer->GetSpecification().Height;
		pContext->SetDynamicConstantBufferView((uint32_t)RootBindings::CommonCBV, sizeof(TAAPass), &taaPass);//maybe cause damage face
		pContext->SetRenderTarget(pLightFrame->m_pColorBuffers[0]->GetRTV());
		pContext->TransitionResource(*(pLightFrame->m_pColorBuffers[0]), ResourceStates::RenderTarget);
		pContext->SetVertexBuffer(0, m_QuadVertexBuffer->GetVBV());
		pContext->SetIndexBuffer(m_QuadIndexBuffer->GetIBV());
		pContext->DrawIndexed(m_QuadIndexBuffer->GetCount());
		//------TAA------

		//for (uint32_t i = 0; i < pLightFrame->m_pColorBuffers.size(); ++i)
		//{
		//	pGraphicsContext->TransitionResource(*pLightFrame->m_pColorBuffers[i], ResourceStates::Common);
		//}
		//pGraphicsContext->TransitionResource(*pLightFrame->m_pDepthBuffer, ResourceStates::Common);

		////------default hdr texture------
		//std::string texturePath = "assets/textures/hdr/brown_photostudio_01_1k.hdr";
		////------default hdr texture------

		//InitializeAndConvertHDRToCubeMap(texturePath);

		Ref<DirectXColorBuffer> pBlurTexture = std::static_pointer_cast<DirectXColorBuffer>(m_BlurTexture);
		pContext->CopyBuffer(*pBlurTexture, *(pLightFrame->m_pColorBuffers[1]));

		m_lastWidth = m_Width;
		m_lastHeight = m_Height;

		//------use for TAA------
		m_CbufferGeometryPass.previousViewProjection = gViewProjection;
		//copy the current scene buffer to previous buffer
		pContext->CopyBuffer(*m_PreviousScene, *m_CurrentScene);
		//------use for TAA------
	}

	void DirectXRenderer::DeferredRendering(Ref<Context> pGraphicsContext, Camera* pCamera, TransformComponent* pCameraTransformComponent, std::vector<TransformComponent*> trans, std::vector<StaticMeshComponent*> meshs, std::vector<MaterialComponent*> materials, 
	std::vector<LightComponent*> lights, std::vector<TransformComponent*> lightTrans, Ref<Framebuffer> pFrameBuffer, Ref<Framebuffer> pLightFrameBuffer, std::vector<int32_t>& entityIds, Ref<Scene> scene)
	{
		m_Width = pFrameBuffer->GetSpecification().Width;
		m_Height = pFrameBuffer->GetSpecification().Height;
		if (m_lastHeight != m_Height || m_lastWidth != m_Width)
		{
			uint32_t widthAndHeight[2] = { m_Width, m_Height };
			m_editorImageWidthHeightBuffer = CreateRef<DirectXGpuBuffer>();
			std::static_pointer_cast<DirectXGpuBuffer>(m_editorImageWidthHeightBuffer)->Create(L"ImageWidthBuffer", 2, sizeof(int32_t), &widthAndHeight);

			m_BlurTexture = CreateRef<DirectXColorBuffer>();
			std::static_pointer_cast<DirectXColorBuffer>(m_BlurTexture)->Create(L"BlurTexture", m_Width, m_Height, 0, ImageFormat::PX_FORMAT_R16G16B16A16_FLOAT, nullptr);

			m_BlurTexture2 = CreateRef<DirectXColorBuffer>();
			std::static_pointer_cast<DirectXColorBuffer>(m_BlurTexture2)->Create(L"BlurTexture2", m_Width, m_Height, 0, ImageFormat::PX_FORMAT_R16G16B16A16_FLOAT, nullptr);

			m_PickerBuffer = CreateRef<StructuredBuffer>();
			std::static_pointer_cast<StructuredBuffer>(m_PickerBuffer)->Create(L"PickerBuffer", m_Width * m_Height, sizeof(int32_t), nullptr);

			//------use for TAA------
			m_PreviousScene = CreateRef<DirectXColorBuffer>();
			std::static_pointer_cast<DirectXColorBuffer>(m_PreviousScene)->Create(L"PreviousScene", m_Width, m_Height, 0, ImageFormat::PX_FORMAT_R16G16B16A16_FLOAT, nullptr);

			m_CurrentScene = CreateRef<DirectXColorBuffer>();
			std::static_pointer_cast<DirectXColorBuffer>(m_CurrentScene)->Create(L"CurrentScene", m_Width, m_Height, 0, ImageFormat::PX_FORMAT_R16G16B16A16_FLOAT, nullptr);
			//------use for TAA------
		}

		Ref<GraphicsContext> pContext = std::static_pointer_cast<GraphicsContext>(pGraphicsContext);

		pContext->SetRootSignature(*m_pDeferredShadingRootSignature);

		Ref<DirectXFrameBuffer> pDirectxFrameBuffer = std::static_pointer_cast<DirectXFrameBuffer>(pFrameBuffer);
		PX_CORE_ASSERT(pDirectxFrameBuffer->m_pColorBuffers.size() == 6, "color buffer's size is not equal to 6!");

		//------shadow map------
		LightComponent* mainDirectLight = nullptr;//find the direct light to generate shadow map
		TransformComponent* mainDirectLightComponent = nullptr;
		for (uint32_t i = 0; i < lights.size(); ++i)
		{
			if (lights[i]->GenerateShadowMap)
			{
				mainDirectLight = lights[i];
				mainDirectLightComponent = lightTrans[i];
				break;
			}
		}

		glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);

		if (mainDirectLight != nullptr)
		{
			pContext->SetPipelineState(*m_RenderShadowMapPso);
			pContext->SetRootSignature(*m_RenderShadowMapRootSignature);
			pContext->SetPrimitiveTopology(PrimitiveTopology::TRIANGLELIST);
			m_ShadowMap->BeginRendering(*pContext);

			float nearPlane = 0.01f;
			float farPlane = 500.0f;

			glm::mat4 lightProjection = glm::transpose(glm::orthoLH_ZO(-20.0f, 20.0f, -20.0f, 20.0f, nearPlane, farPlane));
			glm::mat4 lightView = glm::transpose(glm::inverse(mainDirectLightComponent->GetGlobalTransform(scene->GetRegistry())));
			lightSpaceMatrix = lightView * lightProjection;

			pContext->SetDynamicConstantBufferView((uint32_t)RootBindings::CommonCBV, sizeof(glm::mat4), glm::value_ptr(lightSpaceMatrix));

			for (uint32_t i = 0; i < meshs.size(); ++i)
			{
				if(meshs[i]->m_Model != nullptr)
				//draw every mesh
					meshs[i]->m_Model->DrawShadowMap(trans[i]->GetGlobalTransform(scene->GetRegistry()), pContext, entityIds[i]);
			}

			m_ShadowMap->EndRendering(*pContext);
		}
		//------shadow map------

		//------get the cpu descriptor handle------
		std::vector<Ref<DescriptorCpuHandle>> m_CpuHandles;
		for (uint32_t i = 0; i < pDirectxFrameBuffer->m_pColorBuffers.size(); ++i)
		{
			m_CpuHandles.push_back(pDirectxFrameBuffer->m_pColorBuffers[i]->GetRTV());
		}
		//------get the cpu descriptor handle------

		Ref<DescriptorCpuHandle> dsvHandle = pDirectxFrameBuffer->m_pDepthBuffer->GetDSV();
		pGraphicsContext->SetRenderTargets(6, m_CpuHandles, dsvHandle);

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
		glm::mat4x4 View = glm::transpose(glm::inverse(pCameraTransformComponent->GetGlobalTransform(scene->GetRegistry())));
		glm::mat4x4 Projection = glm::transpose(pCamera->GetProjection());
		glm::mat4x4 gViewProjection = View * Projection;
		m_CbufferGeometryPass.ViewProjection = gViewProjection;
		if (m_IsFirst)
		{
			m_IsFirst = false;
			m_CbufferGeometryPass.previousViewProjection = gViewProjection;
		}
		m_CbufferGeometryPass.frameCount = m_FrameCount;//for taa
		m_CbufferGeometryPass.width = pDirectxFrameBuffer->GetSpecification().Width;
		m_CbufferGeometryPass.height = pDirectxFrameBuffer->GetSpecification().Height;
		++m_FrameCount;
		pContext->SetDynamicConstantBufferView((uint32_t)RootBindings::CommonCBV, sizeof(CbufferGeometryPass), &m_CbufferGeometryPass);

		for (uint32_t i = 0; i < meshs.size(); ++i)
		{
			if (meshs[i]->m_Model != nullptr)
			//draw every mesh
				meshs[i]->m_Model->Draw(trans[i]->GetGlobalTransform(scene->GetRegistry()), pContext, entityIds[i], materials[i]);
		}

		for (uint32_t i = 0; i < pDirectxFrameBuffer->m_pColorBuffers.size() - 1; ++i)
		{
			pGraphicsContext->TransitionResource(*pDirectxFrameBuffer->m_pColorBuffers[i], ResourceStates::Common);
		}
		//editor frame buffer
		pContext->TransitionResource(*(pDirectxFrameBuffer->m_pColorBuffers[5]), ResourceStates::Common);
		pContext->TransitionResource(*(pDirectxFrameBuffer->m_pDepthBuffer), ResourceStates::Common);

		//------second pass:light pass------
		pContext->SetPipelineState(*m_DefaultLightShadingPso);
		pContext->SetRootSignature(*m_pDeferredShadingLightRootSignature);
		//bind render targets
		std::vector<Ref<DescriptorCpuHandle>> m_lightFrameBufferCpuHandles;
		Ref<DirectXFrameBuffer> pLightFrame = std::static_pointer_cast<DirectXFrameBuffer>(pLightFrameBuffer);
		m_lightFrameBufferCpuHandles.push_back(pLightFrame->m_pColorBuffers[0]->GetRTV());
		m_lightFrameBufferCpuHandles.push_back(pLightFrame->m_pColorBuffers[1]->GetRTV());
		dsvHandle = pLightFrame->m_pDepthBuffer->GetDSV();
		pGraphicsContext->SetRenderTargets(2, m_lightFrameBufferCpuHandles, dsvHandle);

		//clear buffer
		for (uint32_t i = 0; i < pLightFrame->m_pColorBuffers.size(); ++i)
		{
			pGraphicsContext->TransitionResource(*pLightFrame->m_pColorBuffers[i], ResourceStates::RenderTarget);
		}
		pGraphicsContext->TransitionResource(*pLightFrame->m_pDepthBuffer, ResourceStates::DepthWrite);

		for (uint32_t i = 0; i < pLightFrame->m_pColorBuffers.size(); ++i)
		{
			//clear color buffer
			pContext->ClearColor(*(pLightFrame->m_pColorBuffers[i]));
		}
		pContext->ClearDepth(*(pLightFrame->m_pDepthBuffer));

		//set viewport and scissor
		pContext->SetViewportAndScissor(vp, scissor);

		m_lightPass.CameraPosition = pCameraTransformComponent->Translation;
		m_lightPass.receiveAmbientLight = false;//test

		std::vector<LightComponent*> ReSortedPointLight;
		std::vector<TransformComponent*> ReSortedPointLightTrans;
		std::vector<LightComponent*> ReSortedDirectLight;
		std::vector<TransformComponent*> ReSortedDirectLightTrans;
		std::vector<LightComponent*> ReSortedSpotLight;
		std::vector<TransformComponent*> ReSortedSpotLightTrans;

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
			else if (lights[i]->lightType == LightType::SpotLight)
			{
				ReSortedSpotLight.push_back(lights[i]);
				ReSortedSpotLightTrans.push_back(lightTrans[i]);
			}
			//---restored point light---
		}

		m_lightPass.PointLightNumber = ReSortedPointLight.size();
		m_lightPass.DirectLightNumber = ReSortedDirectLight.size();
		m_lightPass.SpotLightNumber = ReSortedSpotLight.size();

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
			if (ReSortedDirectLight[i] == mainDirectLight)
			{
				m_lightPass.lights[i + m_lightPass.PointLightNumber].GenerateShadow = 1;
			}
		}

		for (size_t i = 0; i < ReSortedSpotLight.size(); ++i)
		{
			m_lightPass.lights[i + m_lightPass.PointLightNumber + m_lightPass.DirectLightNumber].Position = (*ReSortedSpotLightTrans[i]).Translation;
			m_lightPass.lights[i + m_lightPass.PointLightNumber + m_lightPass.DirectLightNumber].Direction = (*ReSortedSpotLightTrans[i]).Rotation;
			m_lightPass.lights[i + m_lightPass.PointLightNumber + m_lightPass.DirectLightNumber].CutOff = glm::cos(glm::radians((*ReSortedSpotLight[i]).CutOff));
		}

		if (mainDirectLight != nullptr)
		{
			m_lightPass.LightSpaceMatrix = lightSpaceMatrix;
		}

		pContext->SetDynamicConstantBufferView((uint32_t)RootBindings::CommonCBV, sizeof(LightPass), &m_lightPass);

		Ref<DirectXCubeTexture> m_pIrradianceCubeTexture = std::static_pointer_cast<DirectXCubeTexture>(m_irradianceCubeTexture);

		//copy descriptor handle
		Device::Get()->CopyDescriptorsSimple(1, m_DeferredShadingLightGbufferTextureHandles[0].GetCpuHandle(), pDirectxFrameBuffer->m_pColorBuffers[0]->GetSRV(), DescriptorHeapType::CBV_UAV_SRV);
		Device::Get()->CopyDescriptorsSimple(1, m_DeferredShadingLightGbufferTextureHandles[1].GetCpuHandle(), pDirectxFrameBuffer->m_pColorBuffers[1]->GetSRV(), DescriptorHeapType::CBV_UAV_SRV);
		Device::Get()->CopyDescriptorsSimple(1, m_DeferredShadingLightGbufferTextureHandles[2].GetCpuHandle(), pDirectxFrameBuffer->m_pColorBuffers[2]->GetSRV(), DescriptorHeapType::CBV_UAV_SRV);
		Device::Get()->CopyDescriptorsSimple(1, m_DeferredShadingLightGbufferTextureHandles[3].GetCpuHandle(), pDirectxFrameBuffer->m_pColorBuffers[3]->GetSRV(), DescriptorHeapType::CBV_UAV_SRV);
		Device::Get()->CopyDescriptorsSimple(1, m_DeferredShadingLightGbufferTextureHandles[4].GetCpuHandle(), pDirectxFrameBuffer->m_pColorBuffers[4]->GetSRV(), DescriptorHeapType::CBV_UAV_SRV);
		Device::Get()->CopyDescriptorsSimple(1, m_DeferredShadingLightGbufferTextureHandles[5].GetCpuHandle(), m_pIrradianceCubeTexture->GetSrvHandle()->GetCpuHandle(), DescriptorHeapType::CBV_UAV_SRV);
		Device::Get()->CopyDescriptorsSimple(1, m_DeferredShadingLightGbufferTextureHandles[6].GetCpuHandle(), m_prefilterMap->GetSrvHandle()->GetCpuHandle(), DescriptorHeapType::CBV_UAV_SRV);
		Device::Get()->CopyDescriptorsSimple(1, m_DeferredShadingLightGbufferTextureHandles[7].GetCpuHandle(), m_LutTexture->GetHandle()->GetCpuHandle(), DescriptorHeapType::CBV_UAV_SRV);
		Device::Get()->CopyDescriptorsSimple(1, m_DeferredShadingLightGbufferTextureHandles[8].GetCpuHandle(), m_ShadowMap->GetDepthSRV(), DescriptorHeapType::CBV_UAV_SRV);
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

		Device::Get()->CopyDescriptorsSimple(1, m_irradianceCubeTextureHandle->GetCpuHandle(), m_CubeMapTexture->GetSrvHandle()->GetCpuHandle(), DescriptorHeapType::CBV_UAV_SRV);

		pContext->SetDescriptorHeap(DescriptorHeapType::CBV_UAV_SRV, m_irradianceCubeTextureHeap);
		pContext->SetDescriptorTable((uint32_t)RootBindings::MaterialSRVs, m_irradianceCubeTextureHandle->GetGpuHandle());

		glm::mat4 NoRotateView = glm::transpose(glm::mat4(glm::mat3(View)));
		Projection = glm::transpose(pCamera->GetProjection());

		glm::mat4 NoRotateViewProjection = NoRotateView * Projection;
		//pContext->SetDynamicConstantBufferView((uint32_t)RootBindings::CommonCBV, sizeof(glm::mat4), glm::value_ptr(glm::transpose(camera.GetViewProjection())));
		pContext->SetDynamicConstantBufferView((uint32_t)RootBindings::CommonCBV, sizeof(glm::mat4), glm::value_ptr(NoRotateViewProjection));
		pContext->SetVertexBuffer(0, m_CubeVertexBuffer->GetVBV());
		pContext->SetIndexBuffer(m_CubeIndexBuffer->GetIBV());
		pContext->DrawIndexed(m_CubeIndexBuffer->GetCount());
		//------draw sky box------

		//------copy current scene------
		pContext->CopyBuffer(*m_CurrentScene, *(pLightFrame->m_pColorBuffers[0]));
		//------copy current scene------

		//------TAA------
		Ref<DirectXFrameBuffer> pGeometryFrameBuffer = std::static_pointer_cast<DirectXFrameBuffer>(pFrameBuffer);
		Ref<DescriptorCpuHandle> pDsv = pGeometryFrameBuffer->m_pDepthBuffer->GetDepthSRV();
		Ref<DescriptorCpuHandle> pCurrentScene = m_CurrentScene->GetSRV();
		Ref<DescriptorCpuHandle> velocityTexture = pGeometryFrameBuffer->m_pColorBuffers[2]->GetSRV();//velocity texture, get from the geometry framebuffer
		Ref<DescriptorCpuHandle> previousScene = m_PreviousScene->GetSRV();
		Device::Get()->CopyDescriptorsSimple(1, m_TAAFirstTextureDescriptorHandles[0].GetCpuHandle(), pCurrentScene, DescriptorHeapType::CBV_UAV_SRV);
		Device::Get()->CopyDescriptorsSimple(1, m_TAAFirstTextureDescriptorHandles[1].GetCpuHandle(), previousScene, DescriptorHeapType::CBV_UAV_SRV);
		Device::Get()->CopyDescriptorsSimple(1, m_TAAFirstTextureDescriptorHandles[2].GetCpuHandle(), velocityTexture, DescriptorHeapType::CBV_UAV_SRV);
		Device::Get()->CopyDescriptorsSimple(1, m_TAAFirstTextureDescriptorHandles[3].GetCpuHandle(), pDsv, DescriptorHeapType::CBV_UAV_SRV);
		pContext->SetDescriptorHeap(DescriptorHeapType::CBV_UAV_SRV, m_TAATextureDescriptorHeap);
		pContext->SetDescriptorTable((uint32_t)RootBindings::MaterialSRVs, m_TAAFirstTextureDescriptorHandle->GetGpuHandle());
		pContext->SetPipelineState(*m_TAAPSO);
		pContext->SetRootSignature(*m_TAARootSignature);
		TAAPass taaPass;
		taaPass.frameCount = m_FrameCount;
		taaPass.Width = pDirectxFrameBuffer->GetSpecification().Width;
		taaPass.Height = pDirectxFrameBuffer->GetSpecification().Height;
		pContext->SetDynamicConstantBufferView((uint32_t)RootBindings::CommonCBV, sizeof(TAAPass), &taaPass);//maybe cause damage face
		pContext->SetRenderTarget(pLightFrame->m_pColorBuffers[0]->GetRTV());
		pContext->TransitionResource(*(pLightFrame->m_pColorBuffers[0]), ResourceStates::RenderTarget);
		pContext->SetVertexBuffer(0, m_QuadVertexBuffer->GetVBV());
		pContext->SetIndexBuffer(m_QuadIndexBuffer->GetIBV());
		pContext->DrawIndexed(m_QuadIndexBuffer->GetCount());
		//------TAA------

		//for (uint32_t i = 0; i < pLightFrame->m_pColorBuffers.size(); ++i)
		//{
		//	pGraphicsContext->TransitionResource(*pLightFrame->m_pColorBuffers[i], ResourceStates::Common);
		//}
		//pGraphicsContext->TransitionResource(*pLightFrame->m_pDepthBuffer, ResourceStates::Common);

		////------default hdr texture------
		//std::string texturePath = "assets/textures/hdr/brown_photostudio_01_1k.hdr";
		////------default hdr texture------

		//InitializeAndConvertHDRToCubeMap(texturePath);

		Ref<DirectXColorBuffer> pBlurTexture = std::static_pointer_cast<DirectXColorBuffer>(m_BlurTexture);
		pContext->CopyBuffer(*pBlurTexture, *(pLightFrame->m_pColorBuffers[1]));

		m_lastWidth = m_Width;
		m_lastHeight = m_Height;

		//------use for TAA------
		m_CbufferGeometryPass.previousViewProjection = gViewProjection;
		//copy the current scene buffer to previous buffer
		pContext->CopyBuffer(*m_PreviousScene, *m_CurrentScene);
		//------use for TAA------
	}

	void DirectXRenderer::RenderImageToBackBuffer(Ref<GpuResource> pDestResource, Ref<GpuResource> pSrcResource, Ref<Context> context)
	{
		Ref<GraphicsContext> pContext = std::static_pointer_cast<GraphicsContext>(context);
		pContext->SetPipelineState(*m_ImageToBackBufferPso);
		pContext->SetRootSignature(*m_ImageToBackBufferRootSignature);
		
		Ref<DirectXColorBuffer> pDestColorBuffer = std::static_pointer_cast<DirectXColorBuffer>(pDestResource);
		Ref<DirectXColorBuffer> pSrcColorBuffer = std::static_pointer_cast<DirectXColorBuffer>(pSrcResource);

		Ref<DescriptorCpuHandle> pHandle = pSrcColorBuffer->GetSRV();

		pContext->SetPrimitiveTopology(PrimitiveTopology::TRIANGLELIST);
		pContext->SetRenderTarget(pDestColorBuffer->GetRTV());
		Device::Get()->CopyDescriptorsSimple(1, m_ImageDescriptorHandle->GetCpuHandle(), pHandle, DescriptorHeapType::CBV_UAV_SRV);
		pContext->SetDescriptorHeap(DescriptorHeapType::CBV_UAV_SRV, m_ImageDescriptorHeap);
		pContext->SetDescriptorTable((uint32_t)RootBindings::MaterialSRVs, m_ImageDescriptorHandle->GetGpuHandle());

		pContext->TransitionResource(*pDestResource, ResourceStates::RenderTarget);
		pContext->TransitionResource(*pSrcResource, ResourceStates::GenericRead);

		//---set viewport and scissor---
		ViewPort vp;
		vp.TopLeftX = 0.0f;
		vp.TopLeftY = 0.0f;
		vp.Width = pDestColorBuffer->GetWidth();
		vp.Height = pDestColorBuffer->GetHeight();
		vp.MaxDepth = 1.0f;
		vp.MinDepth = 0.0f;

		PixelRect scissor;
		scissor.Left = 0;
		scissor.Right = pDestColorBuffer->GetWidth();
		scissor.Top = 0;
		scissor.Bottom = pDestColorBuffer->GetHeight();

		pContext->SetViewportAndScissor(vp, scissor);
		//---set viewport and scissor---

		pContext->SetVertexBuffer(0, m_QuadVertexBuffer->GetVBV());
		pContext->SetIndexBuffer(m_QuadIndexBuffer->GetIBV());
		pContext->DrawIndexed(m_QuadIndexBuffer->GetCount());

		//pContext->TransitionResource(*pDestResource, ResourceStates::Common);
		//pContext->TransitionResource(*pSrcResource, ResourceStates::Common);
	}

	Ref<DescriptorCpuHandle> DirectXRenderer::GetShadowMapSrvHandle()
	{
		return m_ShadowMap->GetDepthSRV();
	}

	void DirectXRenderer::DrawFrustum(Ref<Context> pGraphicsContext, const EditorCamera& editorCamera, Camera* pCamera, TransformComponent* pCameraTransformComponent, Ref<Framebuffer> pFrameBuffer, Ref<Scene> scene)
	{
		if (pCamera != nullptr && pCameraTransformComponent != nullptr)
		{
			glm::vec4 vec4Vertices[8] = {
				//near plane
				{-1.0f, 1.0f, 0.0f, 1.0f},
				{1.0f, 1.0f, 0.0f, 1.0f},
				{-1.0f, -1.0f, 0.0f, 1.0f},
				{1.0f, -1.0f, 0.0f, 1.0f},

				//far plane
				{-1.0f, 1.0f, 1.0f, 1.0f},
				{1.0f, 1.0f, 1.0f, 1.0f},
				{-1.0f, -1.0f, 1.0f, 1.0f},
				{1.0f, -1.0f, 1.0f, 1.0f}
			};

			glm::vec4 worldFrustum[8];
			for (uint32_t i = 0; i < 8; ++i)
			{
				glm::vec4 newVertices = vec4Vertices[i] * glm::transpose(glm::inverse(pCamera->GetProjection())) * glm::transpose(pCameraTransformComponent->GetGlobalTransform(scene->GetRegistry()));
				worldFrustum[i].x = newVertices.x / newVertices.w;
				worldFrustum[i].y = newVertices.y / newVertices.w;
				worldFrustum[i].z = newVertices.z / newVertices.w;
			}
			float newVertices[24];
			for (uint32_t i = 0; i < 8; ++i)
			{
				newVertices[i * 3] = worldFrustum[i].x;
				newVertices[i * 3 + 1] = worldFrustum[i].y;
				newVertices[i * 3 + 2] = worldFrustum[i].z;
			}

			uint16_t indices[48] = {
				1, 0,
				1, 3,
				3, 2,
				0, 2,//near plane

				1, 5,
				5, 7,
				7, 3,
				3, 1,//right plane

				0, 4,
				4, 6,
				6, 2,
				2, 0,//left plane

				0, 1,
				1, 5,
				5, 4,
				4, 0,//top plane

				2, 3,
				3, 7,
				7, 6,
				6, 2,//down plane,

				6, 7,
				7, 3,
				3, 2,
				2, 6//far plane
			};

			pGraphicsContext->SetPipelineState(*m_CameraFrustumPso);
			pGraphicsContext->SetRootSignature(*m_CameraFrustumRootSignature);
			pGraphicsContext->SetPrimitiveTopology(PrimitiveTopology::LINELIST);

			Ref<DirectXFrameBuffer> pDirectXFrameBuffer = std::static_pointer_cast<DirectXFrameBuffer>(pFrameBuffer);

			std::vector<Ref<DescriptorCpuHandle>> rtvHandles;
			rtvHandles.push_back(pDirectXFrameBuffer->m_pColorBuffers[0]->GetRTV());

			pGraphicsContext->SetRenderTargets(1, rtvHandles);

			//transition resource
			pGraphicsContext->TransitionResource(*(pDirectXFrameBuffer->m_pColorBuffers[0]), ResourceStates::RenderTarget);
			//pGraphicsContext->TransitionResource(*(pDirectXFrameBuffer->m_pDepthBuffer), ResourceStates::DepthWrite);
			//set viewport and scissor
			ViewPort vp;
			vp.TopLeftX = 0.0f;
			vp.TopLeftY = 0.0f;
			vp.Width = pFrameBuffer->GetSpecification().Width;
			vp.Height = pFrameBuffer->GetSpecification().Height;
			vp.MaxDepth = 1.0f;
			vp.MinDepth = 0.0f;

			PixelRect scissor;
			scissor.Left = 0;
			scissor.Right = pFrameBuffer->GetSpecification().Width;
			scissor.Top = 0;
			scissor.Bottom = pFrameBuffer->GetSpecification().Height;

			pGraphicsContext->SetViewportAndScissor(vp, scissor);

			glm::mat4 viewProjection = glm::transpose(editorCamera.GetViewProjection());
			//bind resource
			pGraphicsContext->SetDynamicConstantBufferView((uint32_t)RootBindings::CommonCBV, sizeof(glm::mat4), glm::value_ptr(viewProjection));

			//------test-------
			/*float line[24] = { 0.0f, 0.0f, 0.0f, 0.5, 0.5f, 1.0f, 0.5f, 0.5f, 1.0f, 3.0f, 3.4f, 2.5f,
			0.0f, 0.0f, 0.0f, 0.5, 0.5f, 1.0f, 0.5f, 0.5f, 1.0f, 3.0f, 3.4f, 2.5f };
			uint16_t lineIndex[8] = { 0, 1, 2, 3, 4, 5, 6, 7};*/
			//------test-------


			pGraphicsContext->SetDynamicVB(0, 8, sizeof(float) * 3, newVertices);
			pGraphicsContext->SetDynamicIB(48, indices);
			pGraphicsContext->DrawIndexed(48);

			//test
			//pGraphicsContext->DrawIndexed(48);
		}		
	}

	void DirectXRenderer::CreateRenderImageToBackBufferPipeline()
	{
		m_ImageToBackBufferVs = Shader::Create("assets/shaders/ImageToBackBuffer.hlsl", "VS", "vs_5_0");
		m_ImageToBackBufferPs = Shader::Create("assets/shaders/ImageToBackBuffer.hlsl", "PS", "ps_5_0");

		m_ImageToBackBufferPso = PSO::CreateGraphicsPso(L"ImageToBackBufferPso");
		auto [ImageToBackBufferVsShaderBinary, ImageToBackBufferVsShaderSize] = std::static_pointer_cast<DirectXShader>(m_ImageToBackBufferVs)->GetShaderBinary();
		auto [ImageToBackBufferPsShaderBinary, ImageToBackBufferPsShaderSize] = std::static_pointer_cast<DirectXShader>(m_ImageToBackBufferPs)->GetShaderBinary();

		m_ImageToBackBufferPso->SetVertexShader(ImageToBackBufferVsShaderBinary, ImageToBackBufferVsShaderSize);
		m_ImageToBackBufferPso->SetPixelShader(ImageToBackBufferPsShaderBinary, ImageToBackBufferPsShaderSize);

		Ref<SamplerDesc> samplerDesc = SamplerDesc::Create();
		Ref<DepthState> pDepthState = DepthState::Create();
		Ref<BlenderState> pBlendState = BlenderState::Create();
		Ref<RasterState> pRasterState = RasterState::Create();
		pDepthState->DepthTest(false);

		m_ImageToBackBufferRootSignature = RootSignature::Create((uint32_t)RootBindings::NumRootBindings, 1);
		m_ImageToBackBufferRootSignature->InitStaticSampler(0, samplerDesc, ShaderVisibility::Pixel);
		(*m_ImageToBackBufferRootSignature)[(size_t)RootBindings::MeshConstants].InitAsConstantBuffer(0, ShaderVisibility::Vertex);//root descriptor, only need to bind virtual address
		(*m_ImageToBackBufferRootSignature)[(size_t)RootBindings::MaterialConstants].InitAsConstantBuffer(2, ShaderVisibility::Pixel);
		(*m_ImageToBackBufferRootSignature)[(size_t)RootBindings::MaterialSRVs].InitAsDescriptorRange(RangeType::SRV, 0, 10, ShaderVisibility::ALL);
		(*m_ImageToBackBufferRootSignature)[(size_t)RootBindings::MaterialSamplers].InitAsDescriptorRange(RangeType::SAMPLER, 1, 10, ShaderVisibility::Pixel);
		(*m_ImageToBackBufferRootSignature)[(size_t)RootBindings::CommonSRVs].InitAsDescriptorRange(RangeType::SRV, 10, 10, ShaderVisibility::Pixel);
		(*m_ImageToBackBufferRootSignature)[(size_t)RootBindings::CommonCBV].InitAsConstantBuffer(1, ShaderVisibility::ALL);
		(*m_ImageToBackBufferRootSignature)[(size_t)RootBindings::SkinMatrices].InitiAsBufferSRV(20, ShaderVisibility::ALL);
		m_ImageToBackBufferRootSignature->Finalize(L"ImageToBackBufferRootSignature", RootSignatureFlag::AllowInputAssemblerInputLayout);

		m_ImageToBackBufferPso->SetRootSignature(m_convolutionRootSignature);

		m_ImageToBackBufferPso->SetBlendState(pBlendState);
		m_ImageToBackBufferPso->SetDepthState(pDepthState);
		m_ImageToBackBufferPso->SetRasterizerState(pRasterState);

		std::vector<ImageFormat> imageFormats = { ImageFormat::PX_FORMAT_R8G8B8A8_UNORM };
		m_ImageToBackBufferPso->SetRenderTargetFormats(1, imageFormats.data(), ImageFormat::PX_FORMAT_UNKNOWN);
		m_ImageToBackBufferPso->SetPrimitiveTopologyType(PiplinePrimitiveTopology::TRIANGLE);

		BufferLayout layout = { {ShaderDataType::Float3, "Position", Semantics::POSITION, false}, {ShaderDataType::Float2, "TexCoord", Semantics::TEXCOORD, false} };

		D3D12_INPUT_ELEMENT_DESC* ImageToBackBufferElementArray = new D3D12_INPUT_ELEMENT_DESC[layout.GetElements().size()];

		uint32_t i = 0;
		for (auto& buffElement : layout)
		{
			std::string temp = SemanticsToDirectXSemantics(buffElement.m_sematics);
			ImageToBackBufferElementArray[i].SemanticName = new char[temp.size() + 1];
			std::string temp2(temp.size() + 1, '\0');
			for (uint32_t j = 0; j < temp.size(); ++j)
				temp2[j] = temp[j];
			memcpy((void*)ImageToBackBufferElementArray[i].SemanticName, temp2.c_str(), temp2.size());
			//ElementArray[i].SemanticName = SemanticsToDirectXSemantics(buffElement.m_sematics).c_str();
			ImageToBackBufferElementArray[i].SemanticIndex = 0;
			ImageToBackBufferElementArray[i].Format = ShaderDataTypeToDXGIFormat(buffElement.Type);
			ImageToBackBufferElementArray[i].InputSlot = 0;
			ImageToBackBufferElementArray[i].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
			ImageToBackBufferElementArray[i].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			ImageToBackBufferElementArray[i].InstanceDataStepRate = 0;

			++i;
		}

		std::static_pointer_cast<GraphicsPSO>(m_ImageToBackBufferPso)->SetInputLayout(layout.GetElements().size(), ImageToBackBufferElementArray);

		m_ImageToBackBufferPso->Finalize();
	}

	void DirectXRenderer::CreateRenderShadowMapPipeline()
	{
		m_RenderShadowMapVs = Shader::Create("assets/shaders/ShadowMap.hlsl", "VS", "vs_5_0");
		m_RenderShadowMapPs = Shader::Create("assets/shaders/ShadowMap.hlsl", "PS", "ps_5_0");

		m_RenderShadowMapPso = PSO::CreateGraphicsPso(L"RenderShadowMapPso");
		auto [RenderShadowMapVsShaderBinary, RenderShadowMapVsShaderSize] = std::static_pointer_cast<DirectXShader>(m_RenderShadowMapVs)->GetShaderBinary();
		auto [RenderShadowMapPsShaderBinary, RenderShadowMapPsShaderSize] = std::static_pointer_cast<DirectXShader>(m_RenderShadowMapPs)->GetShaderBinary();

		m_RenderShadowMapPso->SetVertexShader(RenderShadowMapVsShaderBinary, RenderShadowMapVsShaderSize);
		m_RenderShadowMapPso->SetPixelShader(RenderShadowMapPsShaderBinary, RenderShadowMapPsShaderSize);

		Ref<SamplerDesc> samplerDesc = SamplerDesc::Create();
		Ref<DepthState> pDepthState = DepthState::Create();
		Ref<BlenderState> pBlendState = BlenderState::Create();
		Ref<RasterState> pRasterState = RasterState::Create();

		//samplerDesc->SetBorderColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
		samplerDesc->SetTextureAddressMode(AddressMode::BORDER);

		m_RenderShadowMapRootSignature = RootSignature::Create((uint32_t)RootBindings::NumRootBindings, 1);
		m_RenderShadowMapRootSignature->InitStaticSampler(0, samplerDesc, ShaderVisibility::Pixel);
		(*m_RenderShadowMapRootSignature)[(size_t)RootBindings::MeshConstants].InitAsConstantBuffer(0, ShaderVisibility::Vertex);//root descriptor, only need to bind virtual address
		(*m_RenderShadowMapRootSignature)[(size_t)RootBindings::MaterialConstants].InitAsConstantBuffer(2, ShaderVisibility::Pixel);
		(*m_RenderShadowMapRootSignature)[(size_t)RootBindings::MaterialSRVs].InitAsDescriptorRange(RangeType::SRV, 0, 10, ShaderVisibility::ALL);
		(*m_RenderShadowMapRootSignature)[(size_t)RootBindings::MaterialSamplers].InitAsDescriptorRange(RangeType::SAMPLER, 1, 10, ShaderVisibility::Pixel);
		(*m_RenderShadowMapRootSignature)[(size_t)RootBindings::CommonSRVs].InitAsDescriptorRange(RangeType::SRV, 10, 10, ShaderVisibility::Pixel);
		(*m_RenderShadowMapRootSignature)[(size_t)RootBindings::CommonCBV].InitAsConstantBuffer(1, ShaderVisibility::ALL);
		(*m_RenderShadowMapRootSignature)[(size_t)RootBindings::SkinMatrices].InitiAsBufferSRV(20, ShaderVisibility::ALL);
		m_RenderShadowMapRootSignature->Finalize(L"RenderShadowMapRootSignature", RootSignatureFlag::AllowInputAssemblerInputLayout);

		m_RenderShadowMapPso->SetRootSignature(m_RenderShadowMapRootSignature);

		m_RenderShadowMapPso->SetBlendState(pBlendState);
		m_RenderShadowMapPso->SetDepthState(pDepthState);
		m_RenderShadowMapPso->SetRasterizerState(pRasterState);

		m_RenderShadowMapPso->SetDepthTargetFormat(ImageFormat::PX_FORMAT_D16_UNORM);
		m_RenderShadowMapPso->SetPrimitiveTopologyType(PiplinePrimitiveTopology::TRIANGLE);

		BufferLayout layout = { {ShaderDataType::Float3, "Position", Semantics::POSITION, false}};

		D3D12_INPUT_ELEMENT_DESC* RenderShadowMapElementArray = new D3D12_INPUT_ELEMENT_DESC[layout.GetElements().size()];

		uint32_t i = 0;
		for (auto& buffElement : layout)
		{
			std::string temp = SemanticsToDirectXSemantics(buffElement.m_sematics);
			RenderShadowMapElementArray[i].SemanticName = new char[temp.size() + 1];
			std::string temp2(temp.size() + 1, '\0');
			for (uint32_t j = 0; j < temp.size(); ++j)
				temp2[j] = temp[j];
			memcpy((void*)RenderShadowMapElementArray[i].SemanticName, temp2.c_str(), temp2.size());
			//ElementArray[i].SemanticName = SemanticsToDirectXSemantics(buffElement.m_sematics).c_str();
			RenderShadowMapElementArray[i].SemanticIndex = 0;
			RenderShadowMapElementArray[i].Format = ShaderDataTypeToDXGIFormat(buffElement.Type);
			RenderShadowMapElementArray[i].InputSlot = 0;
			RenderShadowMapElementArray[i].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
			RenderShadowMapElementArray[i].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			RenderShadowMapElementArray[i].InstanceDataStepRate = 0;

			++i;
		}

		std::static_pointer_cast<GraphicsPSO>(m_RenderShadowMapPso)->SetInputLayout(layout.GetElements().size(), RenderShadowMapElementArray);

		m_RenderShadowMapPso->Finalize();
	}

	void DirectXRenderer::CreateCameraFrustumPipeline()
	{
		m_CameraFrustumVs = Shader::Create("assets/shaders/CameraFrustum.hlsl", "VS", "vs_5_0");
		m_CameraFrustumPs = Shader::Create("assets/shaders/CameraFrustum.hlsl", "PS", "ps_5_0");

		m_CameraFrustumPso = PSO::CreateGraphicsPso(L"CameraFrustumPso");
		auto [CameraFrustumVsShaderBinary, CameraFrustumVsShaderSize] = std::static_pointer_cast<DirectXShader>(m_CameraFrustumVs)->GetShaderBinary();
		auto [CameraFrustumPsShaderBinary, CameraFrustumPsShaderSize] = std::static_pointer_cast<DirectXShader>(m_CameraFrustumPs)->GetShaderBinary();

		m_CameraFrustumPso->SetVertexShader(CameraFrustumVsShaderBinary, CameraFrustumVsShaderSize);
		m_CameraFrustumPso->SetPixelShader(CameraFrustumPsShaderBinary, CameraFrustumPsShaderSize);

		Ref<SamplerDesc> samplerDesc = SamplerDesc::Create();
		Ref<DepthState> pDepthState = DepthState::Create();
		Ref<BlenderState> pBlendState = BlenderState::Create();
		Ref<RasterState> pRasterState = RasterState::Create();
		pDepthState->DepthTest(false);

		//samplerDesc->SetBorderColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
		//samplerDesc->SetTextureAddressMode(AddressMode::BORDER);

		m_CameraFrustumRootSignature = RootSignature::Create((uint32_t)RootBindings::NumRootBindings, 1);
		m_CameraFrustumRootSignature->InitStaticSampler(0, samplerDesc, ShaderVisibility::Pixel);
		(*m_CameraFrustumRootSignature)[(size_t)RootBindings::MeshConstants].InitAsConstantBuffer(0, ShaderVisibility::Vertex);//root descriptor, only need to bind virtual address
		(*m_CameraFrustumRootSignature)[(size_t)RootBindings::MaterialConstants].InitAsConstantBuffer(2, ShaderVisibility::Pixel);
		(*m_CameraFrustumRootSignature)[(size_t)RootBindings::MaterialSRVs].InitAsDescriptorRange(RangeType::SRV, 0, 10, ShaderVisibility::ALL);
		(*m_CameraFrustumRootSignature)[(size_t)RootBindings::MaterialSamplers].InitAsDescriptorRange(RangeType::SAMPLER, 1, 10, ShaderVisibility::Pixel);
		(*m_CameraFrustumRootSignature)[(size_t)RootBindings::CommonSRVs].InitAsDescriptorRange(RangeType::SRV, 10, 10, ShaderVisibility::Pixel);
		(*m_CameraFrustumRootSignature)[(size_t)RootBindings::CommonCBV].InitAsConstantBuffer(1, ShaderVisibility::ALL);
		(*m_CameraFrustumRootSignature)[(size_t)RootBindings::SkinMatrices].InitiAsBufferSRV(20, ShaderVisibility::ALL);
		m_CameraFrustumRootSignature->Finalize(L"CameraFrustumRootSignature", RootSignatureFlag::AllowInputAssemblerInputLayout);

		m_CameraFrustumPso->SetRootSignature(m_CameraFrustumRootSignature);

		m_CameraFrustumPso->SetBlendState(pBlendState);
		m_CameraFrustumPso->SetDepthState(pDepthState);
		m_CameraFrustumPso->SetRasterizerState(pRasterState);

		std::vector<ImageFormat> imageFormats = { ImageFormat::PX_FORMAT_R16G16B16A16_FLOAT };
		m_CameraFrustumPso->SetRenderTargetFormats(1, imageFormats.data(), ImageFormat::PX_FORMAT_UNKNOWN);
		m_CameraFrustumPso->SetPrimitiveTopologyType(PiplinePrimitiveTopology::LINE);

		BufferLayout layout = { {ShaderDataType::Float3, "Position", Semantics::POSITION, false} };

		D3D12_INPUT_ELEMENT_DESC* CameraFrustumElementArray = FromBufferLayoutToCreateDirectXVertexLayout(layout);

		std::static_pointer_cast<GraphicsPSO>(m_CameraFrustumPso)->SetInputLayout(layout.GetElements().size(), CameraFrustumElementArray);

		m_CameraFrustumPso->Finalize();
	}

	void DirectXRenderer::CreateBlurPipeline()
	{
		m_HorzBlurShader = Shader::Create("assets/shaders/Blur.hlsl", "horzBlurCS", "cs_5_0");
		auto [CsBinary, CsBinarySize] = std::static_pointer_cast<DirectXShader>(m_HorzBlurShader)->GetShaderBinary();

		Ref<SamplerDesc> sampler = SamplerDesc::Create();

		m_HorzBlurPso = CreateRef<ComputePSO>(L"Horz Blur PSO");
		m_HorzBlurPso->SetComputeShader(CsBinary, CsBinarySize);
		m_BlurRootSignature = RootSignature::Create(3, 1);
		m_BlurRootSignature->InitStaticSampler(0, sampler, ShaderVisibility::ALL);
		(*m_BlurRootSignature)[0].InitAsConstants(0, 12, ShaderVisibility::ALL);
		(*m_BlurRootSignature)[1].InitAsDescriptorTable({ std::make_tuple(RangeType::SRV, 0, 1) }, ShaderVisibility::ALL);
		(*m_BlurRootSignature)[2].InitAsDescriptorTable({ std::make_tuple(RangeType::UAV, 0, 1) }, ShaderVisibility::ALL);
		m_BlurRootSignature->Finalize(L"Blur RootSignature", RootSignatureFlag::AllowInputAssemblerInputLayout);
		m_HorzBlurPso->SetRootSignature(m_BlurRootSignature);
		m_HorzBlurPso->Finalize();

		m_VertBlurShader = Shader::Create("assets/shaders/VertBlur.hlsl", "VertBlurCS", "cs_5_0");
		auto [VertCsBinary, VertCsBinarySize] = std::static_pointer_cast<DirectXShader>(m_VertBlurShader)->GetShaderBinary();

		m_VertBlurPso = CreateRef<ComputePSO>(L"Vert Blur PSO");
		m_VertBlurPso->SetComputeShader(VertCsBinary, VertCsBinarySize);
		m_Blur2RootSignature = RootSignature::Create(3, 2);
		m_Blur2RootSignature->InitStaticSampler(0, sampler, ShaderVisibility::ALL);
		m_Blur2RootSignature->InitStaticSampler(1, sampler, ShaderVisibility::ALL);//hash collider
		(*m_Blur2RootSignature)[0].InitAsConstants(0, 12, ShaderVisibility::ALL);
		(*m_Blur2RootSignature)[1].InitAsDescriptorTable({ std::make_tuple(RangeType::SRV, 0, 1) }, ShaderVisibility::ALL);
		(*m_Blur2RootSignature)[2].InitAsDescriptorTable({ std::make_tuple(RangeType::UAV, 0, 1) }, ShaderVisibility::ALL);
		m_Blur2RootSignature->Finalize(L"Blur RootSignature", RootSignatureFlag::AllowInputAssemblerInputLayout);
		m_VertBlurPso->SetRootSignature(m_Blur2RootSignature);
		m_VertBlurPso->Finalize();
	}

	void DirectXRenderer::CreateAdditiveBlendingPipeline()
	{
		m_AdditiveBlendingVs = Shader::Create("assets/shaders/AdditiveBlending.hlsl", "VS", "vs_5_0");
		m_AdditiveBlendingPs = Shader::Create("assets/shaders/AdditiveBlending.hlsl", "PS", "ps_5_0");

		m_AdditiveBlendingPso = PSO::CreateGraphicsPso(L"AdditiveBlendingPso");
		auto [AdditiveBlendingVsShaderBinary, AdditiveBlendingVsShaderSize] = std::static_pointer_cast<DirectXShader>(m_AdditiveBlendingVs)->GetShaderBinary();
		auto [AdditiveBlendingPsShaderBinary, AdditiveBlendingPsShaderSize] = std::static_pointer_cast<DirectXShader>(m_AdditiveBlendingPs)->GetShaderBinary();

		m_AdditiveBlendingPso->SetVertexShader(AdditiveBlendingVsShaderBinary, AdditiveBlendingVsShaderSize);
		m_AdditiveBlendingPso->SetPixelShader(AdditiveBlendingPsShaderBinary, AdditiveBlendingPsShaderSize);

		Ref<SamplerDesc> samplerDesc = SamplerDesc::Create();
		Ref<DepthState> pDepthState = DepthState::Create();
		Ref<BlenderState> pBlendState = BlenderState::Create();
		Ref<RasterState> pRasterState = RasterState::Create();
		pDepthState->DepthTest(false);

		m_AdditiveRootSignature = RootSignature::Create((uint32_t)RootBindings::NumRootBindings, 1);
		m_AdditiveRootSignature->InitStaticSampler(0, samplerDesc, ShaderVisibility::Pixel);
		(*m_AdditiveRootSignature)[(size_t)RootBindings::MeshConstants].InitAsConstantBuffer(0, ShaderVisibility::Vertex);//root descriptor, only need to bind virtual address
		(*m_AdditiveRootSignature)[(size_t)RootBindings::MaterialConstants].InitAsConstantBuffer(2, ShaderVisibility::Pixel);
		(*m_AdditiveRootSignature)[(size_t)RootBindings::MaterialSRVs].InitAsDescriptorRange(RangeType::SRV, 0, 10, ShaderVisibility::ALL);
		(*m_AdditiveRootSignature)[(size_t)RootBindings::MaterialSamplers].InitAsDescriptorRange(RangeType::SAMPLER, 1, 10, ShaderVisibility::Pixel);
		(*m_AdditiveRootSignature)[(size_t)RootBindings::CommonSRVs].InitAsDescriptorRange(RangeType::SRV, 10, 10, ShaderVisibility::Pixel);
		(*m_AdditiveRootSignature)[(size_t)RootBindings::CommonCBV].InitAsConstantBuffer(1, ShaderVisibility::ALL);
		(*m_AdditiveRootSignature)[(size_t)RootBindings::SkinMatrices].InitiAsBufferSRV(20, ShaderVisibility::ALL);
		m_AdditiveRootSignature->Finalize(L"AdditiveBlendingRootSignature", RootSignatureFlag::AllowInputAssemblerInputLayout);

		m_AdditiveBlendingPso->SetRootSignature(m_AdditiveRootSignature);

		m_AdditiveBlendingPso->SetBlendState(pBlendState);
		m_AdditiveBlendingPso->SetDepthState(pDepthState);
		m_AdditiveBlendingPso->SetRasterizerState(pRasterState);

		std::vector<ImageFormat> imageFormats = { ImageFormat::PX_FORMAT_R8G8B8A8_UNORM };
		m_AdditiveBlendingPso->SetRenderTargetFormats(1, imageFormats.data(), ImageFormat::PX_FORMAT_UNKNOWN);
		m_AdditiveBlendingPso->SetPrimitiveTopologyType(PiplinePrimitiveTopology::TRIANGLE);

		BufferLayout layout = { {ShaderDataType::Float3, "Position", Semantics::POSITION, false}, {ShaderDataType::Float2, "TexCoord", Semantics::TEXCOORD, false} };

		D3D12_INPUT_ELEMENT_DESC* AdditiveBlendingElementArray = new D3D12_INPUT_ELEMENT_DESC[layout.GetElements().size()];

		uint32_t i = 0;
		for (auto& buffElement : layout)
		{
			std::string temp = SemanticsToDirectXSemantics(buffElement.m_sematics);
			AdditiveBlendingElementArray[i].SemanticName = new char[temp.size() + 1];
			std::string temp2(temp.size() + 1, '\0');
			for (uint32_t j = 0; j < temp.size(); ++j)
				temp2[j] = temp[j];
			memcpy((void*)AdditiveBlendingElementArray[i].SemanticName, temp2.c_str(), temp2.size());
			//ElementArray[i].SemanticName = SemanticsToDirectXSemantics(buffElement.m_sematics).c_str();
			AdditiveBlendingElementArray[i].SemanticIndex = 0;
			AdditiveBlendingElementArray[i].Format = ShaderDataTypeToDXGIFormat(buffElement.Type);
			AdditiveBlendingElementArray[i].InputSlot = 0;
			AdditiveBlendingElementArray[i].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
			AdditiveBlendingElementArray[i].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			AdditiveBlendingElementArray[i].InstanceDataStepRate = 0;

			++i;
		}

		std::static_pointer_cast<GraphicsPSO>(m_AdditiveBlendingPso)->SetInputLayout(layout.GetElements().size(), AdditiveBlendingElementArray);

		m_AdditiveBlendingPso->Finalize();
	}

	void DirectXRenderer::CreatePointLightVolumePipeline()
	{
		m_PointLightVolumeVs = Shader::Create("assets/shaders/debug/DrawPointLightVolume.hlsl", "VS", "vs_5_0");
		m_PointLightVolumePs = Shader::Create("assets/shaders/debug/DrawPointLightVolume.hlsl", "PS", "ps_5_0");

		m_PointLightVolumePso = PSO::CreateGraphicsPso(L"PointLightVolumePso");
		auto [PointLightVolumeVsShaderBinary, PointLightVolumeVsShaderSize] = std::static_pointer_cast<DirectXShader>(m_PointLightVolumeVs)->GetShaderBinary();
		auto [PointLightVolumePsShaderBinary, PointLightVolumePsShaderSize] = std::static_pointer_cast<DirectXShader>(m_PointLightVolumePs)->GetShaderBinary();

		m_PointLightVolumePso->SetVertexShader(PointLightVolumeVsShaderBinary, PointLightVolumeVsShaderSize);
		m_PointLightVolumePso->SetPixelShader(PointLightVolumePsShaderBinary, PointLightVolumePsShaderSize);

		Ref<SamplerDesc> samplerDesc = SamplerDesc::Create();
		Ref<DepthState> pDepthState = DepthState::Create();
		Ref<BlenderState> pBlendState = BlenderState::Create();
		Ref<RasterState> pRasterState = RasterState::Create();
		pDepthState->DepthTest(false);

		m_PointLightVolumeRootSignature = RootSignature::Create((uint32_t)RootBindings::NumRootBindings, 1);
		m_PointLightVolumeRootSignature->InitStaticSampler(0, samplerDesc, ShaderVisibility::Pixel);
		(*m_PointLightVolumeRootSignature)[(size_t)RootBindings::MeshConstants].InitAsConstantBuffer(0, ShaderVisibility::Vertex);//root descriptor, only need to bind virtual address
		(*m_PointLightVolumeRootSignature)[(size_t)RootBindings::MaterialConstants].InitAsConstantBuffer(2, ShaderVisibility::Pixel);
		(*m_PointLightVolumeRootSignature)[(size_t)RootBindings::MaterialSRVs].InitAsDescriptorRange(RangeType::SRV, 0, 10, ShaderVisibility::ALL);
		(*m_PointLightVolumeRootSignature)[(size_t)RootBindings::MaterialSamplers].InitAsDescriptorRange(RangeType::SAMPLER, 1, 10, ShaderVisibility::Pixel);
		(*m_PointLightVolumeRootSignature)[(size_t)RootBindings::CommonSRVs].InitAsDescriptorRange(RangeType::SRV, 10, 10, ShaderVisibility::Pixel);
		(*m_PointLightVolumeRootSignature)[(size_t)RootBindings::CommonCBV].InitAsConstantBuffer(1, ShaderVisibility::ALL);
		(*m_PointLightVolumeRootSignature)[(size_t)RootBindings::SkinMatrices].InitiAsBufferSRV(20, ShaderVisibility::ALL);
		m_PointLightVolumeRootSignature->Finalize(L"PointLightVolumeRootSignature", RootSignatureFlag::AllowInputAssemblerInputLayout);

		m_PointLightVolumePso->SetRootSignature(m_PointLightVolumeRootSignature);
		
		m_PointLightVolumePso->SetBlendState(pBlendState);
		m_PointLightVolumePso->SetDepthState(pDepthState);
		m_PointLightVolumePso->SetRasterizerState(pRasterState);

		std::vector<ImageFormat> imageFormats = { ImageFormat::PX_FORMAT_R16G16B16A16_FLOAT };
		m_PointLightVolumePso->SetRenderTargetFormats(1, imageFormats.data(), ImageFormat::PX_FORMAT_UNKNOWN);
		m_PointLightVolumePso->SetPrimitiveTopologyType(PiplinePrimitiveTopology::LINE);

		BufferLayout layout = { {ShaderDataType::Float3, "Position", Semantics::POSITION, false}};

		D3D12_INPUT_ELEMENT_DESC* PointLightVolumeElementArray = FromBufferLayoutToCreateDirectXVertexLayout(layout);

		std::static_pointer_cast<GraphicsPSO>(m_PointLightVolumePso)->SetInputLayout(layout.GetElements().size(), PointLightVolumeElementArray);

		m_PointLightVolumePso->Finalize();
	}

	void DirectXRenderer::CreateOutlinePipeline()
	{
		m_OutlineVs = Shader::Create("assets/shaders/debug/OutLine.hlsl", "VS", "vs_5_0");
		m_OutlinePs = Shader::Create("assets/shaders/debug/OutLine.hlsl", "PS", "ps_5_0");

		m_OutlinePso = PSO::CreateGraphicsPso(L"OutlinePso");
		auto [OutLineVsShaderBinary, OutLineVsShaderSize] = std::static_pointer_cast<DirectXShader>(m_OutlineVs)->GetShaderBinary();
		auto [OutLinePsShaderBinary, OutLinePsShaderSize] = std::static_pointer_cast<DirectXShader>(m_OutlinePs)->GetShaderBinary();

		m_OutlinePso->SetVertexShader(OutLineVsShaderBinary, OutLineVsShaderSize);
		m_OutlinePso->SetPixelShader(OutLinePsShaderBinary, OutLinePsShaderSize);

		Ref<SamplerDesc> samplerDesc = SamplerDesc::Create();
		Ref<DepthState> pDepthState = DepthState::Create();
		Ref<BlenderState> pBlendState = BlenderState::Create();
		Ref<RasterState> pRasterState = RasterState::Create();
		//pDepthState->DepthTest(false);
		pDepthState->DepthTest(true);
		pBlendState->SetRenderTargetBlendState(2, true);
		pBlendState->SetIndependentBlendEnable(true);
		pRasterState->SetCullMode(CullMode::Front);

		m_OutlineRootSignature = RootSignature::Create((uint32_t)RootBindings::NumRootBindings, 2);
		m_OutlineRootSignature->InitStaticSampler(0, samplerDesc, ShaderVisibility::Pixel);
		m_OutlineRootSignature->InitStaticSampler(1, samplerDesc, ShaderVisibility::Pixel);//to solve the hash conflict
		(*m_OutlineRootSignature)[(size_t)RootBindings::MeshConstants].InitAsConstantBuffer(0, ShaderVisibility::Vertex);//root descriptor, only need to bind virtual address
		(*m_OutlineRootSignature)[(size_t)RootBindings::MaterialConstants].InitAsConstantBuffer(2, ShaderVisibility::Pixel);
		(*m_OutlineRootSignature)[(size_t)RootBindings::MaterialSRVs].InitAsDescriptorRange(RangeType::SRV, 0, 10, ShaderVisibility::ALL);
		(*m_OutlineRootSignature)[(size_t)RootBindings::MaterialSamplers].InitAsDescriptorRange(RangeType::SAMPLER, 2, 10, ShaderVisibility::Pixel);
		(*m_OutlineRootSignature)[(size_t)RootBindings::CommonSRVs].InitAsDescriptorRange(RangeType::SRV, 10, 10, ShaderVisibility::Pixel);
		(*m_OutlineRootSignature)[(size_t)RootBindings::CommonCBV].InitAsConstantBuffer(1, ShaderVisibility::ALL);
		(*m_OutlineRootSignature)[(size_t)RootBindings::SkinMatrices].InitiAsBufferSRV(20, ShaderVisibility::ALL);
		m_OutlineRootSignature->Finalize(L"OutLineRootSignature", RootSignatureFlag::AllowInputAssemblerInputLayout);

		m_OutlinePso->SetRootSignature(m_OutlineRootSignature);

		m_OutlinePso->SetBlendState(pBlendState);
		m_OutlinePso->SetDepthState(pDepthState);
		m_OutlinePso->SetRasterizerState(pRasterState);

		std::vector<ImageFormat> imageFormats = { ImageFormat::PX_FORMAT_R16G16B16A16_FLOAT, ImageFormat::PX_FORMAT_R16G16B16A16_FLOAT, ImageFormat::PX_FORMAT_R16G16B16A16_FLOAT, ImageFormat::PX_FORMAT_R8G8B8A8_UNORM,
		ImageFormat::PX_FORMAT_R8G8B8A8_UNORM, ImageFormat::PX_FORMAT_R32_SINT };
		m_OutlinePso->SetRenderTargetFormats(6, imageFormats.data(), ImageFormat::PX_FORMAT_D24_UNORM_S8_UINT);
		m_OutlinePso->SetPrimitiveTopologyType(PiplinePrimitiveTopology::TRIANGLE);

		BufferLayout layout = { {ShaderDataType::Float3, "Position", Semantics::POSITION, false},
			{ShaderDataType::Float2, "Texcoord", Semantics::TEXCOORD, false},
			{ShaderDataType::Float3, "Normal", Semantics::NORMAL, false}};

		D3D12_INPUT_ELEMENT_DESC* OutLineElementArray = FromBufferLayoutToCreateDirectXVertexLayout(layout);

		std::static_pointer_cast<GraphicsPSO>(m_OutlinePso)->SetInputLayout(layout.GetElements().size(), OutLineElementArray);

		m_OutlinePso->Finalize();
	}

	void DirectXRenderer::CreateTAAPipeline()
	{
		m_TAAVs = Shader::Create("assets/shaders/AA/TAA.hlsl", "VS", "vs_5_0");
		m_TAAPs = Shader::Create("assets/shaders/AA/TAA.hlsl", "PS", "ps_5_0");

		m_TAAPSO = PSO::CreateGraphicsPso(L"OutlinePso");
		auto [TAAVsShaderBinary, TAAVsShaderSize] = std::static_pointer_cast<DirectXShader>(m_TAAVs)->GetShaderBinary();
		auto [TAAPsShaderBinary, TAAPsShaderSize] = std::static_pointer_cast<DirectXShader>(m_TAAPs)->GetShaderBinary();

		m_TAAPSO->SetVertexShader(TAAVsShaderBinary, TAAVsShaderSize);
		m_TAAPSO->SetPixelShader(TAAPsShaderBinary, TAAPsShaderSize);

		Ref<SamplerDesc> samplerDesc = SamplerDesc::Create();
		Ref<DepthState> pDepthState = DepthState::Create();
		Ref<BlenderState> pBlendState = BlenderState::Create();
		Ref<RasterState> pRasterState = RasterState::Create();
		pDepthState->DepthTest(false);
		//pBlendState->SetRenderTargetBlendState(2, true);
		//pBlendState->SetIndependentBlendEnable(true);
		//pRasterState->SetCullMode(CullMode::Front);

		m_TAARootSignature = RootSignature::Create((uint32_t)RootBindings::NumRootBindings, 1);
		m_TAARootSignature->InitStaticSampler(0, samplerDesc, ShaderVisibility::Pixel);
		(*m_TAARootSignature)[(size_t)RootBindings::MeshConstants].InitAsConstantBuffer(0, ShaderVisibility::Vertex);//root descriptor, only need to bind virtual address
		(*m_TAARootSignature)[(size_t)RootBindings::MaterialConstants].InitAsConstantBuffer(2, ShaderVisibility::Pixel);
		(*m_TAARootSignature)[(size_t)RootBindings::MaterialSRVs].InitAsDescriptorRange(RangeType::SRV, 0, 10, ShaderVisibility::ALL);
		(*m_TAARootSignature)[(size_t)RootBindings::MaterialSamplers].InitAsDescriptorRange(RangeType::SAMPLER, 1, 10, ShaderVisibility::Pixel);
		(*m_TAARootSignature)[(size_t)RootBindings::CommonSRVs].InitAsDescriptorRange(RangeType::SRV, 10, 10, ShaderVisibility::Pixel);
		(*m_TAARootSignature)[(size_t)RootBindings::CommonCBV].InitAsConstantBuffer(1, ShaderVisibility::ALL);
		(*m_TAARootSignature)[(size_t)RootBindings::SkinMatrices].InitiAsBufferSRV(20, ShaderVisibility::ALL);
		m_TAARootSignature->Finalize(L"TAARootSignature", RootSignatureFlag::AllowInputAssemblerInputLayout);

		m_TAAPSO->SetRootSignature(m_TAARootSignature);

		m_TAAPSO->SetBlendState(pBlendState);
		m_TAAPSO->SetDepthState(pDepthState);
		m_TAAPSO->SetRasterizerState(pRasterState);

		std::vector<ImageFormat> imageFormats = { ImageFormat::PX_FORMAT_R16G16B16A16_FLOAT };
		m_TAAPSO->SetRenderTargetFormats(1, imageFormats.data(), ImageFormat::PX_FORMAT_UNKNOWN);
		m_TAAPSO->SetPrimitiveTopologyType(PiplinePrimitiveTopology::TRIANGLE);

		BufferLayout layout = { {ShaderDataType::Float3, "Position", Semantics::POSITION, false},
			{ShaderDataType::Float2, "Texcoord", Semantics::TEXCOORD, false}};

		D3D12_INPUT_ELEMENT_DESC* TAAElementArray = FromBufferLayoutToCreateDirectXVertexLayout(layout);

		std::static_pointer_cast<GraphicsPSO>(m_TAAPSO)->SetInputLayout(layout.GetElements().size(), TAAElementArray);

		m_TAAPSO->Finalize();
	}

	void DirectXRenderer::RenderBlurTexture(Ref<Context> pComputeContext, Ref<Framebuffer> pLightFrameBuffer)
	{
		Ref<DirectXColorBuffer> pBlurTexture = std::static_pointer_cast<DirectXColorBuffer>(m_BlurTexture);
		Ref<DirectXColorBuffer> pBlurTexture2 = std::static_pointer_cast<DirectXColorBuffer>(m_BlurTexture2);
		//copy descriptor
		std::static_pointer_cast<DirectXDevice>(Device::Get())->CopyDescriptorsSimple(1, m_BlurTextureSrvHandle->GetCpuHandle(), pBlurTexture->GetSRV(), DescriptorHeapType::CBV_UAV_SRV);
		std::static_pointer_cast<DirectXDevice>(Device::Get())->CopyDescriptorsSimple(1, m_BlurTexture2UavHandle->GetCpuHandle(), pBlurTexture2->GetUAV(), DescriptorHeapType::CBV_UAV_SRV);

		pComputeContext->SetType(CommandListType::Graphics);

		pComputeContext->SetDescriptorHeap(DescriptorHeapType::CBV_UAV_SRV, m_BlurTextureUavSrvHeap);

		//pComputeContext->TransitionResource(*m_BlurTexture, ResourceStates::GenericRead, true);
		pComputeContext->TransitionResource(*m_BlurTexture2, ResourceStates::UnorderedAccess, true);

		pComputeContext->SetPipelineState(*m_HorzBlurPso);

		pComputeContext->SetRootSignature(*m_BlurRootSignature);

		auto weights = CalcGaussWeights(2.5f);
		int32_t blurRadius = (int32_t)weights.size() / 2;

		pComputeContext->SetConstantArray(0, 1, &blurRadius, 0);
		pComputeContext->SetConstantArray(0, weights.size(), weights.data(), 1);
		pComputeContext->SetDescriptorTable(1, m_BlurTextureSrvHandle->GetGpuHandle());
		pComputeContext->SetDescriptorTable(2, m_BlurTexture2UavHandle->GetGpuHandle());

		pComputeContext->Dispatch2D(m_Width, m_Height, 256, 1);

		//------vertical blur------
		pComputeContext->SetPipelineState(*m_VertBlurPso);
		pComputeContext->SetRootSignature(*m_Blur2RootSignature);
		pComputeContext->SetDescriptorHeap(DescriptorHeapType::CBV_UAV_SRV, m_BlurTextureUavSrvHeap);
		
		std::static_pointer_cast<DirectXDevice>(Device::Get())->CopyDescriptorsSimple(1, m_BlurTexture2SrvHandle->GetCpuHandle(), pBlurTexture2->GetSRV(), DescriptorHeapType::CBV_UAV_SRV);
		std::static_pointer_cast<DirectXDevice>(Device::Get())->CopyDescriptorsSimple(1, m_BlurTextureUavHandle->GetCpuHandle(), pBlurTexture->GetUAV(), DescriptorHeapType::CBV_UAV_SRV);

		pComputeContext->TransitionResource(*m_BlurTexture, ResourceStates::UnorderedAccess, true);

		//bind resource
		pComputeContext->SetConstantArray(0, 1, &blurRadius, 0);
		pComputeContext->SetConstantArray(0, weights.size(), weights.data(), 1);
		pComputeContext->SetDescriptorTable(1, m_BlurTexture2SrvHandle->GetGpuHandle());
		pComputeContext->SetDescriptorTable(2, m_BlurTextureUavHandle->GetGpuHandle());
		pComputeContext->Dispatch2D(m_Width, m_Height, 1, 256);

		pComputeContext->SetType(CommandListType::Compute);
		//------vertical blur------
	}

	void DirectXRenderer::RenderingFinalColorBuffer(Ref<Context> pContext, Ref<Framebuffer> pSceneFrameBuffer, Ref<Framebuffer> pFinalColorBuffer)
	{
		Ref<GraphicsContext> pGraphicsContext = std::static_pointer_cast<GraphicsContext>(pContext);
		pGraphicsContext->SetPipelineState(*m_AdditiveBlendingPso);
		pGraphicsContext->SetRootSignature(*m_AdditiveRootSignature);

		Ref<DirectXFrameBuffer> SceneFrameBuffer = std::static_pointer_cast<DirectXFrameBuffer>(pSceneFrameBuffer);
		Ref<DirectXColorBuffer> pBloomTexture = std::static_pointer_cast<DirectXColorBuffer>(m_BlurTexture);
		Ref<DirectXColorBuffer> pSceneTexture = std::static_pointer_cast<DirectXColorBuffer>(SceneFrameBuffer->m_pColorBuffers[0]);
		Ref<DirectXFrameBuffer> FinalColorBuffer = std::static_pointer_cast<DirectXFrameBuffer>(pFinalColorBuffer);

		pContext->SetPrimitiveTopology(PrimitiveTopology::TRIANGLELIST);
		pContext->SetRenderTarget(FinalColorBuffer->m_pColorBuffers[0]->GetRTV());
		Device::Get()->CopyDescriptorsSimple(1, m_AdditiveBlendingDescriptorHandle->GetCpuHandle(), pSceneTexture->GetSRV(), DescriptorHeapType::CBV_UAV_SRV);
		Device::Get()->CopyDescriptorsSimple(1, m_AdditiveBlendingDescriptorHandle2->GetCpuHandle(), pBloomTexture->GetSRV(), DescriptorHeapType::CBV_UAV_SRV);
		pContext->SetDescriptorHeap(DescriptorHeapType::CBV_UAV_SRV, m_AdditiveBlendingDescriptorHeap);
		pContext->SetDescriptorTable((uint32_t)RootBindings::MaterialSRVs, m_AdditiveBlendingDescriptorHandle->GetGpuHandle());
		pContext->SetDynamicConstantBufferView((uint32_t)RootBindings::CommonCBV, sizeof(float), &m_exposure);
		pContext->TransitionResource(*pSceneTexture, ResourceStates::GenericRead);
		pContext->TransitionResource(*pBloomTexture, ResourceStates::GenericRead);
		pContext->TransitionResource(*(FinalColorBuffer->m_pColorBuffers[0]), ResourceStates::RenderTarget);
		//---set viewport and scissor---
		ViewPort vp;
		vp.TopLeftX = 0.0f;
		vp.TopLeftY = 0.0f;
		vp.Width = SceneFrameBuffer->GetSpecification().Width;
		vp.Height = SceneFrameBuffer->GetSpecification().Height;
		vp.MaxDepth = 1.0f;
		vp.MinDepth = 0.0f;

		PixelRect scissor;
		scissor.Left = 0;
		scissor.Right = SceneFrameBuffer->GetSpecification().Width;
		scissor.Top = 0;
		scissor.Bottom = SceneFrameBuffer->GetSpecification().Height;

		pContext->SetViewportAndScissor(vp, scissor);
		//---set viewport and scissor---

		pContext->SetVertexBuffer(0, m_QuadVertexBuffer->GetVBV());
		pContext->SetIndexBuffer(m_QuadIndexBuffer->GetIBV());
		pContext->DrawIndexed(m_QuadIndexBuffer->GetCount());
	}

	void DirectXRenderer::RenderPointLightVolume(Ref<Context> pGraphicsContext, const EditorCamera& editorCamera, LightComponent* lights, TransformComponent* lightTrans, Ref<Framebuffer> pLightFrameBuffer, Ref<Scene> scene)
	{
		if (lights != nullptr && lightTrans != nullptr)
		{
			pGraphicsContext->SetPipelineState(*m_PointLightVolumePso);
			pGraphicsContext->SetRootSignature(*m_PointLightVolumeRootSignature);
			pGraphicsContext->SetPrimitiveTopology(PrimitiveTopology::LINELIST);

			Ref<DirectXFrameBuffer> pDirectXFrameBuffer = std::static_pointer_cast<DirectXFrameBuffer>(pLightFrameBuffer);
			pGraphicsContext->TransitionResource(*(pDirectXFrameBuffer->m_pColorBuffers[0]), ResourceStates::RenderTarget);
			pGraphicsContext->SetRenderTarget(pDirectXFrameBuffer->m_pColorBuffers[0]->GetRTV());

			glm::mat4 viewProjection = glm::transpose(editorCamera.GetViewProjection());
			//bind resource
			pGraphicsContext->SetDynamicConstantBufferView((uint32_t)RootBindings::MeshConstants, sizeof(glm::mat4), glm::value_ptr(glm::transpose(lights->GetTransformComponent(*lightTrans))));
			pGraphicsContext->SetDynamicConstantBufferView((uint32_t)RootBindings::CommonCBV, sizeof(glm::mat4), glm::value_ptr(viewProjection));
			
			ViewPort vp;
			vp.TopLeftX = 0.0f;
			vp.TopLeftY = 0.0f;
			vp.Width = pLightFrameBuffer->GetSpecification().Width;
			vp.Height = pLightFrameBuffer->GetSpecification().Height;
			vp.MaxDepth = 1.0f;
			vp.MinDepth = 0.0f;

			PixelRect scissor;
			scissor.Left = 0;
			scissor.Right = pLightFrameBuffer->GetSpecification().Width;
			scissor.Top = 0;
			scissor.Bottom = pLightFrameBuffer->GetSpecification().Height;

			pGraphicsContext->SetViewportAndScissor(vp, scissor);
			pGraphicsContext->SetVertexBuffer(0, m_PointLightVolumeVertex->GetVBV());
			pGraphicsContext->SetIndexBuffer(m_PointLightVolumeIndex->GetIBV());
			pGraphicsContext->DrawIndexed(m_PointLightVolumeIndex->GetCount());
		}
	}

	void DirectXRenderer::SetExposure(float exposure)
	{
		m_exposure = exposure;
	}

	void DirectXRenderer::CreateDefaultForwardRendererPso()
	{
		//------Create Default Forward Renderer Pso------
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
		//------Create Default Forward Renderer Pso------
	}

	void DirectXRenderer::CreatePickerPso()
	{
		Ref<SamplerDesc> defaultSampler = SamplerDesc::Create();

		//------Create Picker PSO------
		m_PickerShader = Shader::Create("assets/shaders/ForwardShading/Picker.hlsl", "CSGetPixels", "cs_5_0");
		auto [CsBinary, CsBinarySize] = std::static_pointer_cast<DirectXShader>(m_PickerShader)->GetShaderBinary();
		m_PickerPSO = CreateRef<ComputePSO>(L"Picker PSO");
		m_PickerPSO->SetComputeShader(CsBinary, CsBinarySize);
		m_PickerRootSignature = RootSignature::Create(4, 1);
		m_PickerRootSignature->InitStaticSampler(0, defaultSampler, ShaderVisibility::ALL);
		(*m_PickerRootSignature)[0].InitAsDescriptorTable({ std::make_tuple(RangeType::SRV, 0, 1) }, ShaderVisibility::ALL);
		(*m_PickerRootSignature)[1].InitiAsBufferSRV(1, ShaderVisibility::ALL);
		(*m_PickerRootSignature)[2].InitAsBufferUAV(0, ShaderVisibility::ALL);
		(*m_PickerRootSignature)[3].InitAsConstantBuffer(0, ShaderVisibility::ALL);
		m_PickerRootSignature->Finalize(L"Picker RootSignature", RootSignatureFlag::AllowInputAssemblerInputLayout);
		m_PickerPSO->SetRootSignature(m_PickerRootSignature);
		m_PickerPSO->Finalize();

		m_ComputeSrvHeap = DescriptorHeap::Create(L"Picker Srv Heap", DescriptorHeapType::CBV_UAV_SRV, 1);
		m_TextureHandle = m_ComputeSrvHeap->Alloc(1);
		//------Create Picker PSO------
	}

	void DirectXRenderer::RenderPickerBuffer(Ref<Context> pComputeContext, Ref<Framebuffer> pFrameBuffer)
	{
		//get the editor buffer
		Ref<DirectXFrameBuffer> pDirectxFrameBuffer = std::static_pointer_cast<DirectXFrameBuffer>(pFrameBuffer);
		PX_CORE_ASSERT(pDirectxFrameBuffer->m_pColorBuffers.size() == 6, "frame color buffer's size error!");

		Ref<DirectXColorBuffer> pColorBuffer = pDirectxFrameBuffer->m_pColorBuffers[5];

		//creat the structed buffer
		//m_PickerBuffer = CreateRef<StructuredBuffer>();
		//std::static_pointer_cast<StructuredBuffer>(m_PickerBuffer)->Create(L"PickerBuffer", m_Width * m_Height, sizeof(int32_t), nullptr);

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
		//pComputeContext->Finish(true);
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

		//------Create CubeMap Texture------
		m_CubeMapTexture = CubeTexture::Create(512, 512, ImageFormat::PX_FORMAT_R8G8B8A8_UNORM);
		Ref<DirectXCubeTexture> m_pCubeMapTexture = std::static_pointer_cast<DirectXCubeTexture>(m_CubeMapTexture);
		//------Create CubeMap Texture------
		
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
	//	pContext->TransitionResource(*(m_pCubeMapTexture->m_pCubeTextureResource), ResourceStates::Common);
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

				pContext->SetDynamicConstantBufferView((uint32_t)RootBindings::CommonCBV, sizeof(PrefilterMapPass), &prefilterPass);

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
		//------Create Deferred Geometry Rendere------
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
		m_pDeferredShadingRootSignature->Finalize(L"DeferredShadingGeometryRootSignature", RootSignatureFlag::AllowInputAssemblerInputLayout);
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

		std::vector<ImageFormat> imageFormats = { ImageFormat::PX_FORMAT_R16G16B16A16_FLOAT, ImageFormat::PX_FORMAT_R16G16B16A16_FLOAT, ImageFormat::PX_FORMAT_R16G16B16A16_FLOAT, ImageFormat::PX_FORMAT_R8G8B8A8_UNORM,
		ImageFormat::PX_FORMAT_R8G8B8A8_UNORM, ImageFormat::PX_FORMAT_R32_SINT };
		m_DefaultGeometryShadingPso->SetRenderTargetFormats(6, imageFormats.data(), ImageFormat::PX_FORMAT_D24_UNORM_S8_UINT);
		m_DefaultGeometryShadingPso->SetPrimitiveTopologyType(PiplinePrimitiveTopology::TRIANGLE);

		m_GeometryVertexShader = Shader::Create("assets/shaders/DeferredShading/GeometryPass.hlsl", "VS", "vs_5_0");
		m_GeometryPixelShader = Shader::Create("assets/shaders/DeferredShading/GeometryPass.hlsl", "PS", "ps_5_0");

		auto [VsBinary, VsBinarySize] = std::static_pointer_cast<DirectXShader>(m_GeometryVertexShader)->GetShaderBinary();
		auto [PsBinary, PsBinarySize] = std::static_pointer_cast<DirectXShader>(m_GeometryPixelShader)->GetShaderBinary();
		m_DefaultGeometryShadingPso->SetVertexShader(VsBinary, VsBinarySize);
		m_DefaultGeometryShadingPso->SetPixelShader(PsBinary, PsBinarySize);

		m_DefaultGeometryShadingPso->SetRootSignature(m_pDeferredShadingRootSignature);
		//------Create Deferred Geometry Renderer------

		//------Create Deferred Light Renderer------
		m_DefaultLightShadingPso = PSO::CreateGraphicsPso(L"DeferredShadingLightPso");

		Ref<SamplerDesc> ShadowMapDesc = SamplerDesc::Create();
		ShadowMapDesc->SetBorderColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
		ShadowMapDesc->SetTextureAddressMode(AddressMode::BORDER);

		Ref<SamplerDesc> pointClampDesc = SamplerDesc::Create();
		pointClampDesc->SetTextureAddressMode(AddressMode::CLAMP);

		//-------create root signature------
		m_pDeferredShadingLightRootSignature = RootSignature::Create((uint32_t)RootBindings::NumRootBindings, 3);
		m_pDeferredShadingLightRootSignature->InitStaticSampler(0, samplerDesc, ShaderVisibility::Pixel);
		m_pDeferredShadingLightRootSignature->InitStaticSampler(1, ShadowMapDesc, ShaderVisibility::Pixel);
		m_pDeferredShadingLightRootSignature->InitStaticSampler(2, pointClampDesc, ShaderVisibility::Pixel);
		//m_pDeferredShadingLightRootSignature->InitStaticSampler(2, pointClampDesc, ShaderVisibility::Pixel);
		(*m_pDeferredShadingLightRootSignature)[(size_t)RootBindings::MeshConstants].InitAsConstantBuffer(0, ShaderVisibility::Vertex);//root descriptor, only need to bind virtual address
		(*m_pDeferredShadingLightRootSignature)[(size_t)RootBindings::MaterialConstants].InitAsConstantBuffer(2, ShaderVisibility::Pixel);
		(*m_pDeferredShadingLightRootSignature)[(size_t)RootBindings::MaterialSRVs].InitAsDescriptorRange(RangeType::SRV, 0, 10, ShaderVisibility::ALL);
		(*m_pDeferredShadingLightRootSignature)[(size_t)RootBindings::MaterialSamplers].InitAsDescriptorRange(RangeType::SAMPLER, 3, 10, ShaderVisibility::Pixel);
		(*m_pDeferredShadingLightRootSignature)[(size_t)RootBindings::CommonSRVs].InitAsDescriptorRange(RangeType::SRV, 10, 10, ShaderVisibility::Pixel);
		(*m_pDeferredShadingLightRootSignature)[(size_t)RootBindings::CommonCBV].InitAsConstantBuffer(1, ShaderVisibility::ALL);
		(*m_pDeferredShadingLightRootSignature)[(size_t)RootBindings::SkinMatrices].InitiAsBufferSRV(20, ShaderVisibility::ALL);
		m_pDeferredShadingLightRootSignature->Finalize(L"DeferredShadingLightRootSignature", RootSignatureFlag::AllowInputAssemblerInputLayout);
		//-------create root signature------

		m_DefaultLightShadingPso->SetBlendState(pBlendState);
		m_DefaultLightShadingPso->SetRasterizerState(pRasterState);
		m_DefaultLightShadingPso->SetDepthState(pDepthState);

		imageFormats = { ImageFormat::PX_FORMAT_R16G16B16A16_FLOAT, ImageFormat::PX_FORMAT_R16G16B16A16_FLOAT };
		m_DefaultLightShadingPso->SetRenderTargetFormats(2, imageFormats.data(), ImageFormat::PX_FORMAT_D24_UNORM_S8_UINT);
		m_DefaultLightShadingPso->SetPrimitiveTopologyType(PiplinePrimitiveTopology::TRIANGLE);

		m_LightVertexShader = Shader::Create("assets/shaders/DeferredShading/LightPass.hlsl", "VS", "vs_5_0");
		m_LightPixelShader = Shader::Create("assets/shaders/DeferredShading/LightPass.hlsl", "PS", "ps_5_0");

		auto [LightVsBinary, LightVsBinarySize] = std::static_pointer_cast<DirectXShader>(m_LightVertexShader)->GetShaderBinary();
		auto [LightPsBinary, LightPsBinarySize] = std::static_pointer_cast<DirectXShader>(m_LightPixelShader)->GetShaderBinary();
		m_DefaultLightShadingPso->SetVertexShader(LightVsBinary, LightVsBinarySize);
		m_DefaultLightShadingPso->SetPixelShader(LightPsBinary, LightPsBinarySize);

		m_DefaultLightShadingPso->SetRootSignature(m_pDeferredShadingLightRootSignature);

		BufferLayout layout = { {ShaderDataType::Float3, "Position", Semantics::POSITION, false},
		{ShaderDataType::Float2, "TexCoord", Semantics::TEXCOORD, false}};//for plane

		m_DeferredShadingLightPsoIndex = CreateDeferredLightPso(layout);
		//------Create Deferred Light Renderer------
	}

	void DirectXRenderer::CreateConvertHDRToCubePipeline()
	{
		m_EquirectangularToCubemapVs = Shader::Create("assets/shaders/IBL/EquirectangularToCubemap.hlsl", "VS", "vs_5_0");
		m_EquirectangularToCubemapFs = Shader::Create("assets/shaders/IBL/EquirectangularToCubemap.hlsl", "PS", "ps_5_0");

		auto [VSShaderBinary, VSBinarySize] = std::static_pointer_cast<DirectXShader>(m_EquirectangularToCubemapVs)->GetShaderBinary();
		auto [PSShaderBinary, PSBinarySize] = std::static_pointer_cast<DirectXShader>(m_EquirectangularToCubemapFs)->GetShaderBinary();

		BufferLayout layout = { {ShaderDataType::Float3, "Position", Semantics::POSITION, false} };

		//------layout------
		D3D12_INPUT_ELEMENT_DESC* ElementArray = FromBufferLayoutToCreateDirectXVertexLayout(layout);
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
		D3D12_INPUT_ELEMENT_DESC* SkyBoxElementArray = FromBufferLayoutToCreateDirectXVertexLayout(layout);
		//------element array------

		m_SkyBoxPso->SetRootSignature(m_SkyBoxRootSignature);

		m_SkyBoxPso->SetVertexShader(SkyBoxVSShaderBinary, SkyBoxVSBinarySize);
		m_SkyBoxPso->SetPixelShader(SkyBoxPSShaderBinary, SkyBoxPSBinarySize);

		pDepthState->SetDepthFunc(DepthFunc::LEQUAL);

		m_SkyBoxPso->SetBlendState(pBlendState);
		m_SkyBoxPso->SetRasterizerState(pRasterState);
		m_SkyBoxPso->SetDepthState(pDepthState);

		std::vector<ImageFormat> SkyBoxImageFormats = { ImageFormat::PX_FORMAT_R16G16B16A16_FLOAT };
		m_SkyBoxPso->SetRenderTargetFormats(1, SkyBoxImageFormats.data(), ImageFormat::PX_FORMAT_D24_UNORM_S8_UINT);
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

		D3D12_INPUT_ELEMENT_DESC* ConvolutionElementArray = FromBufferLayoutToCreateDirectXVertexLayout(layout);

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

		D3D12_INPUT_ELEMENT_DESC* PrefilterElementArray = FromBufferLayoutToCreateDirectXVertexLayout(layout);

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

		D3D12_INPUT_ELEMENT_DESC* LutElementArray = FromBufferLayoutToCreateDirectXVertexLayout(layout);

		std::static_pointer_cast<GraphicsPSO>(m_LutPso)->SetInputLayout(layout.GetElements().size(), LutElementArray);

		m_LutPso->Finalize();
	}

}