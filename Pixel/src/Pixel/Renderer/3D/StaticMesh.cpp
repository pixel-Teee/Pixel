#include "pxpch.h"

#include "StaticMesh.h"
#include "Pixel/Renderer/RenderCommand.h"
#include "Pixel/Renderer/UniformBuffer.h"
#include "Pixel/Renderer/3D/Material.h"
#include "Pixel/Core/Application.h"
#include "Pixel/Renderer/BaseRenderer.h"
#include "Pixel/Renderer/Context/Context.h"
#include "Pixel/Renderer/RendererType.h"
#include "Pixel/Renderer/Descriptor/DescriptorHeap.h"
#include "Pixel/Renderer/Descriptor/DescriptorAllocator.h"
#include "Pixel/Renderer/Device/Device.h"

#include <glm/gtc/type_ptr.hpp>

namespace Pixel {

	StaticMesh::StaticMesh()
	{
		for (uint32_t i = 0; i < (uint32_t)Semantics::MAX; ++i)
		{
			m_DataBuffer[i] = nullptr;
			m_DataBufferSize[i] = 0;
		}
		m_Index = nullptr;
		m_IndexSize = 0;
		m_AlternationDataBuffer = nullptr;
		m_AlternationDataBufferSize = 0;

		m_pDescriptorHeap = DescriptorHeap::Create(L"Static Mesh", DescriptorHeapType::CBV_UAV_SRV, 5);
		m_pTextureFirstHandle = m_pDescriptorHeap->Alloc(5);
	}

	StaticMesh::StaticMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t> indices)
	{
		//this->vertices = vertices;
		//this->indices = indices;
		//
		////Create VAO
		//VAO = VertexArray::Create();

		////Create VBO
		//VBO = VertexBuffer::Create(vertices.size() * sizeof(Vertex));
		//VBO->SetLayout(
		//	{
		//		{
		//			{ShaderDataType::Float3, "a_Pos"},
		//			{ShaderDataType::Float3, "a_Normal"},
		//			{ShaderDataType::Float2, "a_TexCoord"},
		//			{ShaderDataType::Int,	 "a_EntityID"}
		//		}
		//	}
		//);

		//VAO->AddVertexBuffer(VBO);

		////Create IBO
		//IBO = IndexBuffer::Create(indices.size());

		//VAO->SetIndexBuffer(IBO);
	}

	StaticMesh::StaticMesh(const StaticMesh& others)
	{
		//TODO:need to refractor
		m_VertexBuffer = others.m_VertexBuffer;
		m_IndexBuffer = others.m_IndexBuffer;

		for (uint32_t i = 0; i < (uint32_t)Semantics::MAX; ++i)
		{
			if (others.m_DataBuffer[i] != nullptr)
			{
				m_DataBuffer[i] = new unsigned char[others.m_DataBufferSize[i]];
				memcpy(m_DataBuffer[i], others.m_DataBuffer[i], others.m_DataBufferSize[i]);
				m_DataBufferSize[i] = others.m_DataBufferSize[i];
			}
			else
			{
				m_DataBuffer[i] = nullptr;
				m_DataBufferSize[i] = 0;
			}
		}

		if (others.m_Index != nullptr)
		{
			m_Index = new unsigned char[others.m_IndexSize];
			memcpy(m_Index, others.m_Index, others.m_IndexSize);
			m_IndexSize = others.m_IndexSize;
		}
		else
		{
			m_Index = nullptr;
			m_IndexSize = 0;
		}

		if (others.m_AlternationDataBuffer != nullptr)
		{
			m_AlternationDataBuffer = new unsigned char[others.m_AlternationDataBufferSize];
			memcpy(m_AlternationDataBuffer, others.m_AlternationDataBuffer, others.m_AlternationDataBufferSize);
			m_AlternationDataBufferSize = others.m_AlternationDataBufferSize;
		}
		else
		{
			m_AlternationDataBuffer = nullptr;
			m_AlternationDataBufferSize = 0;
		}

		PsoIndex = others.PsoIndex;

		m_pDescriptorHeap = DescriptorHeap::Create(L"Static Mesh Textures", DescriptorHeapType::CBV_UAV_SRV, 5);
		m_pTextureFirstHandle = m_pDescriptorHeap->Alloc(5);
	}

	StaticMesh& StaticMesh::operator=(const StaticMesh& rhs)
	{
		if (this == &rhs)
			return *this;

		//delete original data
		for (uint32_t i = 0; i < (uint32_t)Semantics::MAX; ++i)
		{
			if (m_DataBuffer[i] != nullptr)
			{
				delete[] m_DataBuffer[i];
			}
			m_DataBuffer[i] = nullptr;
		}

		if (m_Index != nullptr)
		{
			delete[] m_Index;
			m_Index = nullptr;
		}
		//delete[]Index;
		if (m_AlternationDataBuffer != nullptr)
		{
			delete[] m_AlternationDataBuffer;
			m_AlternationDataBuffer = nullptr;
		}

		//copy other's data
		m_VertexBuffer = rhs.m_VertexBuffer;
		m_IndexBuffer = rhs.m_IndexBuffer;

		for (uint32_t i = 0; i < (uint32_t)Semantics::MAX; ++i)
		{
			if (rhs.m_DataBuffer[i] != nullptr)
			{
				m_DataBuffer[i] = new unsigned char[rhs.m_DataBufferSize[i]];
				memcpy(m_DataBuffer[i], rhs.m_DataBuffer[i], rhs.m_DataBufferSize[i]);
				m_DataBufferSize[i] = rhs.m_DataBufferSize[i];
			}
			else
			{
				m_DataBuffer[i] = nullptr;
				m_DataBufferSize[i] = 0;
			}
		}

		if (rhs.m_Index != nullptr)
		{
			m_Index = new unsigned char[rhs.m_IndexSize];
			memcpy(m_Index, rhs.m_Index, rhs.m_IndexSize);
			m_IndexSize = rhs.m_IndexSize;
		}
		else
		{
			m_Index = nullptr;
			m_IndexSize = 0;
		}

		if (rhs.m_AlternationDataBuffer != nullptr)
		{
			m_AlternationDataBuffer = new unsigned char[rhs.m_AlternationDataBufferSize];
			memcpy(m_AlternationDataBuffer, rhs.m_AlternationDataBuffer, rhs.m_AlternationDataBufferSize);
			m_AlternationDataBufferSize = rhs.m_AlternationDataBufferSize;
		}
		else
		{
			m_AlternationDataBuffer = nullptr;
			m_AlternationDataBufferSize = 0;
		}

		PsoIndex = rhs.PsoIndex;
		m_pDescriptorHeap = DescriptorHeap::Create(L"Static Mesh", DescriptorHeapType::CBV_UAV_SRV, 5);
		m_pTextureFirstHandle = m_pDescriptorHeap->Alloc(5);
	}

	StaticMesh::~StaticMesh()
	{
		for (uint32_t i = 0; i < (uint32_t)Semantics::MAX; ++i)
		{
			if (m_DataBuffer[i] != nullptr)
			{
				delete[] m_DataBuffer[i];
			}
			m_DataBuffer[i] = nullptr;
		}

		if (m_Index != nullptr)
		{
			delete[] m_Index;
			m_Index = nullptr;
		}
		//delete[]Index;
		if (m_AlternationDataBuffer != nullptr)
		{
			delete[] m_AlternationDataBuffer;
			m_AlternationDataBuffer = nullptr;
		}
	}

	void StaticMesh::Draw(const glm::mat4& transform, Ref<Shader>& shader, std::vector<Ref<Texture2D>> textures, int entityID, Ref<UniformBuffer> modelUniformBuffer, bool bEntityDirty)
	{
		//SetupMesh(entityID, bEntityDirty);
		//shader->Bind();
		//glm::mat4 trans = transform;
		////fix:hard code
		//shader->SetMat4("u_Model", transform);
		////modelUniformBuffer->SetData(0, sizeof(glm::mat4), glm::value_ptr(trans));

		////Bind Texture
		//shader->SetInt("tex_Albedo", 0);
		//textures[0]->Bind(0);

		//shader->SetInt("tex_normalMap", 1);
		//textures[1]->Bind(1);

		//shader->SetInt("tex_Specular", 2);
		//textures[2]->Bind(2);

		//shader->SetInt("tex_metallic", 3);
		//textures[3]->Bind(3);

		//shader->SetInt("tex_emissive", 4);
		//textures[4]->Bind(4);

		//VAO->Bind();
		//RenderCommand::DrawIndexed(Primitive::TRIANGLE, VAO, IBO->GetCount());
	}

	void StaticMesh::Draw()
	{
		if (!isFirst)
		{
			isFirst = true;


			m_VertexBuffer->SetData(m_AlternationDataBuffer, m_AlternationDataBufferSize);

			m_IndexBuffer->SetData(m_Index, m_IndexSize / 4);

		}

		//RenderCommand::DrawIndexed(Primitive::TRIANGLE, VAO, IBO->GetCount());
	}

	void StaticMesh::Draw(const glm::mat4& transform, Ref<MaterialInstance> pMaterialInstance, int entityID)
	{
		Ref<Material> pMaterial = pMaterialInstance->GetMaterial();
		
	}

	void StaticMesh::Draw(Ref<Context> pContext, const glm::mat4& transform, int32_t entityId)
	{
		//if (!isFirst)
		//{
		//	isFirst = true;
		//	SetupMesh(entityId, false);
		//}	
		pContext->SetPipelineState(*(Application::Get().GetRenderer()->GetPso(PsoIndex)));

		m_MeshConstant.world = glm::transpose(transform);

		pContext->SetDynamicConstantBufferView((uint32_t)RootBindings::MeshConstants, sizeof(MeshConstant), &m_MeshConstant);

		pContext->SetVertexBuffer(0, m_VertexBuffer->GetVBV());
		pContext->SetIndexBuffer(m_IndexBuffer->GetIBV());
		pContext->DrawIndexed(m_IndexBuffer->GetCount());
	}

	void StaticMesh::Draw(Ref<Context> pContext, const glm::mat4& transform, int32_t entityId, MaterialComponent* pMaterial)
	{
		//if (!isFirst)
		//{
		//	isFirst = true;
		//	SetupMesh(entityId, false);
		//}
		pContext->SetPipelineState(*(Application::Get().GetRenderer()->GetPso(PsoIndex)));

		m_MeshConstant.world = glm::transpose(transform);
		m_MeshConstant.invWorld = glm::transpose(glm::inverse(transform));
		if (isFirst)
		{
			isFirst = false;
			m_MeshConstant.previousWorld = glm::transpose(transform);
		}
		m_MeshConstant.editor = entityId;

		pContext->SetDynamicConstantBufferView((uint32_t)RootBindings::MeshConstants, sizeof(MeshConstant), &m_MeshConstant);

		//get descriptor size
		uint32_t DescriptorSize = Device::Get()->GetDescriptorAllocator((uint32_t)DescriptorHeapType::CBV_UAV_SRV)->GetDescriptorSize();

		std::vector<DescriptorHandle> handles;
		for (uint32_t i = 0; i < 5; ++i)
		{
			DescriptorHandle secondHandle = (*m_pTextureFirstHandle) + i * DescriptorSize;
			handles.push_back(secondHandle);
		}	

		Device::Get()->CopyDescriptorsSimple(1, handles[0].GetCpuHandle(), pMaterial->Albedo->GetCpuDescriptorHandle(), DescriptorHeapType::CBV_UAV_SRV);
		Device::Get()->CopyDescriptorsSimple(1, handles[1].GetCpuHandle(), pMaterial->NormalMap->GetCpuDescriptorHandle(), DescriptorHeapType::CBV_UAV_SRV);
		Device::Get()->CopyDescriptorsSimple(1, handles[2].GetCpuHandle(), pMaterial->Roughness->GetCpuDescriptorHandle(), DescriptorHeapType::CBV_UAV_SRV);
		Device::Get()->CopyDescriptorsSimple(1, handles[3].GetCpuHandle(), pMaterial->Metallic->GetCpuDescriptorHandle(), DescriptorHeapType::CBV_UAV_SRV);
		Device::Get()->CopyDescriptorsSimple(1, handles[4].GetCpuHandle(), pMaterial->Emissive->GetCpuDescriptorHandle(), DescriptorHeapType::CBV_UAV_SRV);
		pContext->SetDescriptorHeap(DescriptorHeapType::CBV_UAV_SRV, m_pDescriptorHeap);
		//bind texture
		pContext->SetDescriptorTable((uint32_t)RootBindings::MaterialSRVs, m_pTextureFirstHandle->GetGpuHandle());

		m_MaterialConstant.Albedo = pMaterial->gAlbedo;
		m_MaterialConstant.Emissive = pMaterial->gEmissive;
		m_MaterialConstant.Metallic = pMaterial->gMetallic;
		m_MaterialConstant.Roughness = pMaterial->gRoughness;
		m_MaterialConstant.HaveNormal = pMaterial->HaveNormal;
		m_MaterialConstant.shadingModel = (uint32_t)pMaterial->shadingModel;
		m_MaterialConstant.ClearCoat = pMaterial->ClearCoat;
		m_MaterialConstant.ClearCoatRoughness = pMaterial->ClearCoatRoughness;//for clear coat

		pContext->SetDynamicConstantBufferView((uint32_t)RootBindings::MaterialConstants, sizeof(MaterialConstant), &m_MaterialConstant);


		pContext->SetVertexBuffer(0, m_VertexBuffer->GetVBV());
		pContext->SetIndexBuffer(m_IndexBuffer->GetIBV());
		pContext->DrawIndexed(m_IndexBuffer->GetCount());

		//unbind descriptor heap
		Ref<DescriptorHeap> nullHeap = DescriptorHeap::Create();
		pContext->SetDescriptorHeap(DescriptorHeapType::CBV_UAV_SRV, nullHeap);

		m_MeshConstant.previousWorld = glm::transpose(transform);
	}

	void StaticMesh::DrawShadowMap(Ref<Context> pContext, const glm::mat4& transform, int32_t entityId)
	{
		//if (!isFirst)
		//{
		//	isFirst = true;
		//	SetupMesh(entityId, false);
		//}

		m_MeshConstant.world = glm::transpose(transform);

		pContext->SetDynamicConstantBufferView((uint32_t)RootBindings::MeshConstants, sizeof(MeshConstant), &m_MeshConstant);

		pContext->SetVertexBuffer(0, m_VertexBuffer->GetVBV());
		pContext->SetIndexBuffer(m_IndexBuffer->GetIBV());
		pContext->DrawIndexed(m_IndexBuffer->GetCount());
	}

	void StaticMesh::DrawOutLine(Ref<Context> pContext, const glm::mat4& transform)
	{
		//pContext->SetPipelineState(*(Application::Get().GetRenderer()->GetPso(PsoIndex)));

		m_MeshConstant.world = glm::transpose(transform);
		m_MeshConstant.invWorld = glm::transpose(glm::inverse(transform));
		if (isFirst)
		{
			isFirst = false;
			m_MeshConstant.previousWorld = glm::transpose(transform);
		}
		m_MeshConstant.editor = -1;
		m_MaterialConstant.shadingModel = (uint32_t)ShadingModel::SM_Unlit;//unlit

		pContext->SetDynamicConstantBufferView((uint32_t)RootBindings::MeshConstants, sizeof(MeshConstant), &m_MeshConstant);
		pContext->SetDynamicConstantBufferView((uint32_t)RootBindings::MaterialConstants, sizeof(MaterialConstant), &m_MaterialConstant);

		pContext->SetVertexBuffer(0, m_VertexBuffer->GetVBV());
		pContext->SetIndexBuffer(m_IndexBuffer->GetIBV());
		pContext->DrawIndexed(m_IndexBuffer->GetCount());

		//record last world
		m_MeshConstant.previousWorld = glm::transpose(transform);
	}

	//Ref<VertexArray> StaticMesh::GetVerterArray()
	//{
	//	return VAO;
	//}

	Ref<VertexBuffer> StaticMesh::GetVertexBuffer()
	{
		return m_VertexBuffer;
	}

	//setup mesh after load model
	void StaticMesh::SetupMesh(int entityID, bool bHavedSwitched)
	{
		//if (EntityID == -1 || bHavedSwitched)
		//{
		//	bHavedSwitched = false;
		//	EntityID = entityID;

		//	uint32_t numVertices = m_DataBufferSize[(uint64_t)Semantics::POSITION] / 12;

		//	uint32_t offset = 0;
		//	uint32_t size = 0;
		//	for (auto element : m_VertexBuffer->GetLayout())
		//	{
		//		if (element.m_sematics == Semantics::Editor)
		//		{
		//			offset = element.Offset;
		//			size = element.Size;
		//			break;
		//		}
		//	}

		//	for (uint32_t i = 0; i < numVertices; ++i)
		//	{
		//		//one int for byte
		//		memcpy(&m_DataBuffer[(uint64_t)Semantics::Editor][i * size], &EntityID, 4);
		//	}

		//	//unsigned char* dataBuffer = new unsigned char[bufferSize];
		//	BufferLayout layout = m_VertexBuffer->GetLayout();
		//	const std::vector<BufferElement>& elements = layout.GetElements();
		//	for (uint32_t i = 0; i < numVertices; ++i)
		//	{
		//		for (uint32_t j = 0; j < elements.size(); ++j)
		//		{
		//			memcpy(&m_AlternationDataBuffer[i * layout.GetStride() + elements[j].Offset],
		//			&m_DataBuffer[(uint64_t)elements[j].m_sematics][i * elements[j].Size], elements[j].Size);
		//		}
		//	}
		//	//delete []dataBuffer;

		//	m_VertexBuffer->SetData(m_AlternationDataBuffer, m_AlternationDataBufferSize);

		//	m_IndexBuffer->SetData(m_Index, m_IndexSize / 4);
		//}	
	}

}