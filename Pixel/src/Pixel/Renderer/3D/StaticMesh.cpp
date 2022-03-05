#include "pxpch.h"

#include "StaticMesh.h"
#include "Pixel/Renderer/RenderCommand.h"

namespace Pixel {

	StaticMesh::StaticMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t> indices)
	{
		this->vertices = vertices;
		this->indices = indices;
		
		//Create VAO
		VAO = VertexArray::Create();

		//Create VBO
		VBO = VertexBuffer::Create(vertices.size() * sizeof(Vertex));
		VBO->SetLayout(
			{
				{
					{ShaderDataType::Float3, "a_Pos"},
					{ShaderDataType::Float3, "a_Normal"},
					{ShaderDataType::Float2, "a_TexCoord"},
					{ShaderDataType::Int,	 "a_EntityID"}
				}
			}
		);

		VAO->AddVertexBuffer(VBO);

		//Create IBO
		IBO = IndexBuffer::Create(indices.size());

		VAO->SetIndexBuffer(IBO);
	}

	void StaticMesh::Draw(const glm::mat4& transform, Ref<Shader>& shader, std::vector<Ref<Texture2D>> textures, int entityID)
	{
		SetupMesh(entityID);
		shader->Bind();
		shader->SetMat4("u_Model", transform);

		//Bind Texture
		shader->SetInt("tex_Albedo", 0);
		textures[0]->Bind(0);

		shader->SetInt("tex_normalMap", 1);
		textures[1]->Bind(1);

		shader->SetInt("tex_Specular", 2);
		textures[2]->Bind(2);

		shader->SetInt("tex_metallic", 3);
		textures[3]->Bind(3);

		shader->SetInt("tex_emissive", 4);
		textures[4]->Bind(4);

		VAO->Bind();
		RenderCommand::DrawIndexed(VAO, IBO->GetCount());
	}

	void StaticMesh::Draw()
	{
		//SetupMesh(EntityID);
		if (!isFirst)
		{
			isFirst = true;
			VAO->Bind();

			VBO->SetData(vertices.data(), sizeof(Vertex) * vertices.size());

			IBO->SetData(indices.data(), indices.size());

			VAO->Unbind();
		}
		VAO->Bind();
		RenderCommand::DrawIndexed(VAO, IBO->GetCount());
	}

	//setup mesh after load model
	void StaticMesh::SetupMesh(int entityID)
	{
		if (EntityID == -1)
		{
			EntityID = entityID;
			VAO->Bind();

			for (int i = 0; i < vertices.size(); ++i)
			{
				vertices[i].EntityID = entityID;
			}

			VBO->SetData(vertices.data(), sizeof(Vertex) * vertices.size());

			IBO->SetData(indices.data(), indices.size());

			VAO->Unbind();
		}	
	}

}