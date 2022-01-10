#include "pxpch.h"

#include "Renderer3D.h"

#include "Pixel/Renderer/VertexArray.h"
#include "Pixel/Renderer/Buffer.h"
#include "Pixel/Renderer/Shader.h"
#include "Pixel/Renderer/RenderCommand.h"

namespace Pixel {

	struct Vertex
	{
		glm::vec3 Pos;
		glm::vec2 Coord;
		glm::vec3 Normal;
	};

	struct RenderObject
	{
		int BaseVertexLocation = -1;
		int StartIndexLocation = -1;
		int EndIndexLocation = -1;
	};

	struct RenderObjectVertexStorage
	{
		static const int MaxTriangles = 30000;
		static const int MaxVertices = MaxTriangles * 3;
		static const uint32_t MaxIndices = MaxVertices * 1000;
		
		Vertex* ObjectVertexBufferPtr = nullptr;
		uint32_t* ObjectIndexBufferPtr = nullptr;

		Ref<VertexArray> m_VertexArray;
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;
		Ref<Shader> m_Shader;
	};

	static RenderObjectVertexStorage RenderObjectDatas;
	
	void Renderer3D::Init()
	{	
		//Create VertexArray
		RenderObjectDatas.m_VertexArray = VertexArray::Create();
		
		//Create VertexBuffer
		RenderObjectDatas.m_VertexBuffer = VertexBuffer::Create(RenderObjectVertexStorage::MaxVertices * sizeof(Vertex));

		RenderObjectDatas.m_VertexBuffer->SetLayout({
			{ShaderDataType::Float3, "a_Pos"},
			{ShaderDataType::Float2, "a_Coord"},
			{ShaderDataType::Float3, "a_Normal"},
			}	
		);
		RenderObjectDatas.m_VertexArray->AddVertexBuffer(RenderObjectDatas.m_VertexBuffer);

		//Create IndexBuffer
		RenderObjectDatas.m_IndexBuffer = IndexBuffer::Create(RenderObjectVertexStorage::MaxIndices);
		RenderObjectDatas.m_VertexArray->SetIndexBuffer(RenderObjectDatas.m_IndexBuffer);

		//Shader
		RenderObjectDatas.m_Shader = Shader::Create("assets/shaders/Common.glsl");
		RenderObjectDatas.m_Shader->Bind();
	}

	void Renderer3D::DrawQube(const glm::mat4& transform)
	{
		float CubeVertices[] = {
			//Pos				Coord			Normal
			-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
			 0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
			 0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			 0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			-0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

			 0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
			 0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			 0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			 0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			 0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f
		};

		int IndicesOffset = 0;
		uint32_t CubeIndices[12] = {0};
		for (int i = 0; i < 2; ++i)
		{	
			CubeIndices[i * 6] = IndicesOffset;
			CubeIndices[i * 6 + 1] = IndicesOffset + 1;
			CubeIndices[i * 6 + 2] = IndicesOffset + 2;
			//CubeIndices[i * 6 + 3] = 
			CubeIndices[i * 6 + 3] = IndicesOffset + 3;
			CubeIndices[i * 6 + 4] = IndicesOffset + 4;
			CubeIndices[i * 6 + 5] = IndicesOffset + 5;
			IndicesOffset += 6;
		}


		RenderObjectDatas.m_VertexBuffer->SetData(CubeVertices, (uint32_t)sizeof(CubeVertices));
		RenderObjectDatas.m_IndexBuffer->SetData(CubeIndices, 12);

		RenderObjectDatas.m_Shader->SetMat4("u_Model", transform);
	}

	void Renderer3D::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		RenderObjectDatas.m_Shader->Bind();
		RenderObjectDatas.m_VertexArray->Bind();

		glm::mat4 viewProj = camera.GetProjection() * glm::inverse(transform);

		RenderObjectDatas.m_Shader->SetMat4("u_ViewProjection", viewProj);
	}

	void Renderer3D::EndScene()
	{
		RenderCommand::DrawIndexed(RenderObjectDatas.m_VertexArray, 12);
	}

	void Renderer3D::Flush()
	{

	}

}