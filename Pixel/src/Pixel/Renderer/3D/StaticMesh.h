#pragma once

#include "BaseType.h"

#include "Pixel/Renderer/VertexArray.h"
#include "Pixel/Renderer/Buffer.h"
#include "Pixel/Renderer/Shader.h"

namespace Pixel {
	class StaticMesh
	{
	public:
		StaticMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t> indices);

		void Draw(const glm::mat4& transform, const Ref<Shader>& shader, int entityID);
	private:
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		
		Ref<VertexArray> VAO;
		Ref<VertexBuffer> VBO;
		Ref<IndexBuffer> IBO;

		int EntityID = -1;

		void SetupMesh(int entityID);
	};
}
