#include "pxpch.h"

#include "Renderer3D.h"

#include "Pixel/Renderer/Texture.h"

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
		static const int MaxIndices = MaxVertices * 1000;
		
		Vertex* ObjectVertexBufferPtr = nullptr;
		uint32_t* ObjectIndexBufferPtr = nullptr;

		Ref<VertexArray> m_VertexArray;
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;
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
			{ShaderDataType::Float3, "a_Color"}
			}	
		);
		RenderObjectDatas.m_VertexArray->AddVertexBuffer(RenderObjectDatas.m_VertexBuffer);

		//Create IndexBuffer
		RenderObjectDatas.m_IndexBuffer = IndexBuffer::Create(RenderObjectDatas.ObjectIndexBufferPtr, RenderObjectDatas.MaxIndices);
		RenderObjectDatas.m_VertexArray->SetIndexBuffer(RenderObjectDatas.m_IndexBuffer);

	}

	void Renderer3D::DrawQube()
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

		float CubeIndices[12] = {0};
		for (int i = 0; i < 2; ++i)
		{	
			CubeIndices[i * 6] = 0;
			CubeIndices[i * 6 + 1] = 1;
			CubeIndices[i * 6 + 2] = 2;
			//CubeIndices[i * 6 + 3] = 
		}
	}

	void Renderer3D::BeginScene()
	{
		
	}

	void Renderer3D::EndScene()
	{

	}

	void Renderer3D::Flush()
	{

	}

}