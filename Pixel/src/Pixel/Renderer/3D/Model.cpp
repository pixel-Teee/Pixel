#include "pxpch.h"

#include "Model.h"
#include "Pixel/Renderer/Texture.h"
#include "Pixel/Renderer/UniformBuffer.h"
#include "Pixel/Renderer/3D/Material.h"
#include "Pixel/Core/Application.h"
#include "Pixel/Renderer/BaseRenderer.h"

namespace Pixel {

	//draw model's every meshes
	void Model::Draw(const glm::mat4& transform, Ref<Shader>& shader, std::vector<Ref<Texture2D>> textures, int entityID, Ref<UniformBuffer> modelUniformBuffer)
	{
		for(unsigned int i = 0; i < m_Meshes.size(); ++i)
			m_Meshes[i]->Draw(transform, shader, textures, entityID, modelUniformBuffer, m_EntityDirty);
	}

	void Model::Draw(const glm::mat4& transform, Ref<MaterialInstance> pMaterialInstance, int entityID)
	{

	}

	void Model::Draw()
	{	
		for(unsigned int i = 0; i < m_Meshes.size(); ++i)
			m_Meshes[i]->Draw();
	}

	void Model::Draw(const glm::mat4& transform, Ref<Context> pContext, int32_t entityId)
	{
		for (uint32_t i = 0; i < m_Meshes.size(); ++i)
			m_Meshes[i]->Draw(pContext, transform, entityId);
	}

	void Model::Draw(const glm::mat4& transform, Ref<Context> pContext, int32_t entityId, 
	MaterialComponent* pMaterialCompoent)
	{
		for (uint32_t i = 0; i < m_Meshes.size(); ++i)
		{
			if(pMaterialCompoent->m_Materials.size() > i && pMaterialCompoent->m_Materials[i] != nullptr)
				m_Meshes[i]->Draw(pContext, transform, entityId, pMaterialCompoent->m_Materials[i]);
		}
	}

	void Model::DrawOutLine(const glm::mat4& transform, Ref<Context> pContext)
	{
		for (uint32_t i = 0; i < m_Meshes.size(); ++i)
			m_Meshes[i]->DrawOutLine(pContext, transform);
	}

	Ref<Model> Model::Create(const std::string& path)
	{
		return std::make_shared<Model>(path);
	}

	void Model::DrawShadowMap(const glm::mat4& transform, Ref<Context> pContext, int32_t entityId)
	{
		for (uint32_t i = 0; i < m_Meshes.size(); ++i)
			m_Meshes[i]->DrawShadowMap(pContext, transform, entityId);
	}

	/*------------------------------------------------------
	----------Load Modle and Populate Vertex Information----
	------------------------------------------------------*/
	void Model::LoadModel(const std::string& path)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			PX_CORE_ASSERT("ERROR::ASSIMP::{0}", importer.GetErrorString());
			return;
		}

		m_directory = path.substr(path.find_last_of("/\\"));

		ProcessNode(scene->mRootNode, scene);
	}

	void Model::ProcessNode(aiNode* node, const aiScene* scene)
	{
		for (uint32_t i = 0; i < node->mNumMeshes; ++i)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			m_Meshes.push_back(ProcessMesh(mesh, scene));
		}

		for (uint32_t i = 0; i < node->mNumChildren; ++i)
		{
			ProcessNode(node->mChildren[i], scene);
		}
	}

	Ref<StaticMesh> Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
	{
		Ref<StaticMesh> staticMesh = CreateRef<StaticMesh>();
		std::vector<BufferElement> elements;

		uint32_t bufferSize = 0;
		//------Allocator Memory------
		if (mesh->HasPositions())
		{
			//position 12 byte
			staticMesh->m_DataBuffer[(uint64_t)Semantics::POSITION] = new unsigned char[mesh->mNumVertices * 3 * 4];
			staticMesh->m_DataBufferSize[(uint64_t)Semantics::POSITION] = mesh->mNumVertices * 3 * 4;
			elements.push_back({ ShaderDataType::Float3, "a_Pos", Semantics::POSITION });
			bufferSize += mesh->mNumVertices * 12;
		}

		if (mesh->HasTextureCoords(0))
		{
			staticMesh->m_DataBuffer[(uint64_t)Semantics::TEXCOORD] = new unsigned char[mesh->mNumVertices * 2 * 4];
			staticMesh->m_DataBufferSize[(uint64_t)Semantics::TEXCOORD] = mesh->mNumVertices * 2 * 4;
			elements.push_back({ ShaderDataType::Float2, "a_TexCoord", Semantics::TEXCOORD });
			bufferSize += mesh->mNumVertices * 8;
		}

		if (mesh->HasNormals())
		{
			staticMesh->m_DataBuffer[(uint64_t)Semantics::NORMAL] = new unsigned char[mesh->mNumVertices * 3 * 4];
			staticMesh->m_DataBufferSize[(uint64_t)Semantics::NORMAL] = mesh->mNumVertices * 3 * 4;
			elements.push_back({ ShaderDataType::Float3, "a_Normal", Semantics::NORMAL });
			bufferSize += mesh->mNumVertices * 12;
		}

		if (mesh->HasTangentsAndBitangents())
		{
			staticMesh->m_DataBuffer[(uint64_t)Semantics::TANGENT] = new unsigned char[mesh->mNumVertices * 3 * 4];
			staticMesh->m_DataBuffer[(uint64_t)Semantics::BINORMAL] = new unsigned char[mesh->mNumVertices * 3 * 4];
			staticMesh->m_DataBufferSize[(uint64_t)Semantics::TANGENT] = mesh->mNumVertices * 3 * 4;
			staticMesh->m_DataBufferSize[(uint64_t)Semantics::BINORMAL] = mesh->mNumVertices * 3 * 4;
			elements.push_back({ ShaderDataType::Float3, "a_Tangent", Semantics::TANGENT });
			elements.push_back({ ShaderDataType::Float3, "a_Binormal", Semantics::BINORMAL });
			bufferSize += mesh->mNumVertices * 24;
		}
		//------Editor------
		//staticMesh->m_DataBuffer[(uint64_t)Semantics::Editor] = new unsigned char[mesh->mNumVertices * 4];
		//staticMesh->m_DataBufferSize[(uint64_t)Semantics::Editor] = mesh->mNumVertices * 4;
		//elements.push_back({ ShaderDataType::Int, "a_EntityID", Semantics::Editor });
		//bufferSize += mesh->mNumVertices * 4;
		//------Editor------
		//------Allocator Memory------

		//staticMesh->m_VertexBuffer = VertexBuffer::Create(bufferSize);

		int32_t id = -1;
		for (uint32_t i = 0; i < mesh->mNumVertices; ++i)
		{
			if (mesh->HasPositions())
			{
				memcpy(&staticMesh->m_DataBuffer[(uint64_t)Semantics::POSITION][i * 12], &mesh->mVertices[i].x, 4);
				memcpy(&staticMesh->m_DataBuffer[(uint64_t)Semantics::POSITION][i * 12 + 4], &mesh->mVertices[i].y, 4);
				memcpy(&staticMesh->m_DataBuffer[(uint64_t)Semantics::POSITION][i * 12 + 8], &mesh->mVertices[i].z, 4);
			}

			if (mesh->HasTextureCoords(0))
			{
				memcpy(&staticMesh->m_DataBuffer[(uint64_t)Semantics::TEXCOORD][i * 8], &mesh->mTextureCoords[0][i].x, 4);
				memcpy(&staticMesh->m_DataBuffer[(uint64_t)Semantics::TEXCOORD][i * 8 + 4], &mesh->mTextureCoords[0][i].y, 4);
			}

			if (mesh->HasNormals())
			{
				memcpy(&staticMesh->m_DataBuffer[(uint64_t)Semantics::NORMAL][i * 12], &mesh->mNormals[i].x, 4);
				memcpy(&staticMesh->m_DataBuffer[(uint64_t)Semantics::NORMAL][i * 12 + 4], &mesh->mNormals[i].y, 4);
				memcpy(&staticMesh->m_DataBuffer[(uint64_t)Semantics::NORMAL][i * 12 + 8], &mesh->mNormals[i].z, 4);
			}

			if (mesh->HasTangentsAndBitangents())
			{
				memcpy(&staticMesh->m_DataBuffer[(uint64_t)Semantics::TANGENT][i * 12], &mesh->mTangents[i].x, 4);
				memcpy(&staticMesh->m_DataBuffer[(uint64_t)Semantics::TANGENT][i * 12 + 4], &mesh->mTangents[i].y, 4);
				memcpy(&staticMesh->m_DataBuffer[(uint64_t)Semantics::TANGENT][i * 12 + 8], &mesh->mTangents[i].z, 4);

				memcpy(&staticMesh->m_DataBuffer[(uint64_t)Semantics::BINORMAL][i * 12], &mesh->mNormals[i].x, 4);
				memcpy(&staticMesh->m_DataBuffer[(uint64_t)Semantics::BINORMAL][i * 12 + 4], &mesh->mNormals[i].y, 4);
				memcpy(&staticMesh->m_DataBuffer[(uint64_t)Semantics::BINORMAL][i * 12 + 8], &mesh->mNormals[i].z, 4);
			}

			//---Editor---
			//memcpy(&staticMesh->m_DataBuffer[(uint64_t)Semantics::Editor][i * 4], &id, 4);
			//---Editor---
		}
		BufferLayout layout{elements};

		//Calculate Offset
		elements = layout.GetElements();

		//------Alternation Copy TO VRM------
		staticMesh->m_AlternationDataBuffer = new unsigned char[bufferSize];
		for (uint32_t i = 0; i < mesh->mNumVertices; ++i)
		{
			for (uint32_t j = 0; j < elements.size(); ++j)
			{
				Semantics semantics = elements[j].m_sematics;

				memcpy(&staticMesh->m_AlternationDataBuffer[i * layout.GetStride() + elements[j].Offset],
				&staticMesh->m_DataBuffer[(uint64_t)semantics][i * elements[j].Size], elements[j].Size);
			}
		}
		//------Alternation Copy TO VRM------
			
		staticMesh->m_VertexBuffer = VertexBuffer::Create((float*)staticMesh->m_AlternationDataBuffer, mesh->mNumVertices, layout.GetStride());
		//staticMesh->m_VertexBuffer->SetData(staticMesh->m_AlternationDataBuffer, bufferSize);
		staticMesh->m_VertexBuffer->SetLayout(layout);
		staticMesh->m_AlternationDataBufferSize = bufferSize;

		uint32_t IndexOffset = 0;
		uint32_t IndexNums = 0;
		staticMesh->m_Index = new unsigned char[mesh->mNumFaces * 3 * 4];
		staticMesh->m_IndexSize = mesh->mNumFaces * 3 * 4;
		for (uint32_t i = 0; i < mesh->mNumFaces; ++i)
		{
			aiFace face = mesh->mFaces[i];
			//one face == one primitive
			for (uint32_t j = 0; j < face.mNumIndices; ++j)
			{
				memcpy(&staticMesh->m_Index[IndexOffset], &face.mIndices[j], sizeof(uint32_t));
				IndexOffset += sizeof(uint32_t);
				++IndexNums;
			}
		}

		staticMesh->m_IndexBuffer = IndexBuffer::Create((uint32_t*)staticMesh->m_Index, IndexNums);

		//create pso
		//staticMesh->PsoIndex = Application::Get().GetRenderer()->CreatePso(layout);
		staticMesh->PsoIndex = Application::Get().GetRenderer()->CreateDeferredPso(layout);
		staticMesh->TransParentPsoIndex = Application::Get().GetRenderer()->CreatePso(layout);
		//std::vector<Vertex> vertices;
		//std::vector<uint32_t> indices;

		//for (uint32_t i = 0; i < mesh->mNumVertices; ++i)
		//{
		//	Vertex vertex;
		//	glm::vec3 vector;

		//	if (mesh->HasPositions())
		//	{		
		//		vector.x = mesh->mVertices[i].x;
		//		vector.y = mesh->mVertices[i].y;
		//		vector.z = mesh->mVertices[i].z;
		//		vertex.Pos = vector;
		//	}

		//	if (mesh->HasNormals())
		//	{
		//		vector.x = mesh->mNormals[i].x;
		//		vector.y = mesh->mNormals[i].y;
		//		vector.z = mesh->mNormals[i].z;
		//		vertex.Normal = vector;
		//	}
		//	
		//	if (mesh->HasTangentsAndBitangents())
		//	{
		//		vector.x = mesh->mTangents[i].x;
		//		vector.y = mesh->mTangents[i].y;
		//		vector.z = mesh->mTangents[i].z;
		//		//vertex.Tangent = vector;
		//	}
		//	//tangent
		//	//vector.x = mesh->mTangents[i].x;
		//	//vector.y = mesh->mTangents[i].y;
		//	//vector.z = mesh->mTangents[i].z;
		//	//vertex.Tangent = vector;

		//	//tex coord
		//	if (mesh->mTextureCoords[0])
		//	{
		//		glm::vec2 vec;
		//		vec.x = mesh->mTextureCoords[0][i].x;
		//		vec.y = mesh->mTextureCoords[0][i].y;
		//		vertex.TexCoord = vec;
		//	}
		//	else
		//		vertex.TexCoord = glm::vec2(0.0f, 0.0f);

		//	vertex.EntityID = -1;

		//	vertices.push_back(vertex);
		//}

		//for (uint32_t i = 0; i < mesh->mNumFaces; ++i)
		//{
		//	aiFace face = mesh->mFaces[i];
		//	for (uint32_t j = 0; j < face.mNumIndices; ++j)
		//	{
		//		indices.push_back(face.mIndices[j]);
		//	}
		//}

		return staticMesh;
	}

	REFLECT_STRUCT_BEGIN(Model)
	REFLECT_STRUCT_MEMBER(m_directory)
	REFLECT_STRUCT_END()
}