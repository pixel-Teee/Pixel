#pragma once

#include "BaseType.h"

#include "Pixel/Renderer/VertexArray.h"
#include "Pixel/Renderer/Buffer.h"
#include "Pixel/Renderer/Shader.h"
#include "Pixel/Renderer/Texture.h"
#include "Pixel/Renderer/UniformBuffer.h"

namespace Pixel {

	class StaticMesh
	{
	public:
		StaticMesh();
		StaticMesh(const StaticMesh& others);
		~StaticMesh();

		StaticMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t> indices);

		void Draw(const glm::mat4& transform, Ref<Shader>& shader, std::vector<Ref<Texture2D>> textures, int entityID, Ref<UniformBuffer> modelUniformBuffer);

		//TODO:temporary forward draw
		void Draw();

		Ref<VertexArray> GetVerterArray();
		Ref<VertexBuffer> GetVertexBuffer();
	private:
		/*------Data------*/
		unsigned char* m_DataBuffer[(uint64_t)Semantics::MAX];
		uint32_t m_DataBufferSize[(uint64_t)Semantics::MAX];
		unsigned char* m_Index = nullptr;
		uint32_t m_IndexSize = 0;
		unsigned char* m_AlternationDataBuffer = nullptr;
		uint32_t m_AlternationDataBufferSize = 0;//total databuffer size
		/*------Data------*/
		Ref<VertexArray> VAO;
		Ref<VertexBuffer> VBO;
		Ref<IndexBuffer> IBO;

		int EntityID = -1;

		//temporary for forward draw
		bool isFirst = false;

		void SetupMesh(int entityID);

		friend class Model;
	};
}
