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
		VBO = VertexBuffer::Create(sizeof(Vertex) * vertices.size());
		VBO->SetLayout(
			{
				{
					{ShaderDataType::Float3, "a_Pos"},
					{ShaderDataType::Float3, "a_Normal"},
					{ShaderDataType::Float3, "a_Tangent"},
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

	void StaticMesh::Draw(const glm::mat4& transform, const Ref<Shader>& shader, int entityID)
	{
		SetupMesh(entityID);
		shader->Bind();
		shader->SetMat4("u_Model", (transform));
		VAO->Bind();
		RenderCommand::DrawIndexed(VAO, IBO->GetCount());
	}

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