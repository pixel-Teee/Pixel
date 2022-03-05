#pragma once

#include "BaseType.h"

#include "Pixel/Renderer/VertexArray.h"
#include "Pixel/Renderer/Buffer.h"
#include "Pixel/Renderer/Shader.h"
#include "Pixel/Renderer/Texture.h"

namespace Pixel {

	class StaticMesh
	{
	public:
		StaticMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t> indices);

		void Draw(const glm::mat4& transform, Ref<Shader>& shader, std::vector<Ref<Texture2D>> textures, int entityID);

		//TODO:temporary forward draw
		void Draw();
	private:
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		
		Ref<VertexArray> VAO;
		Ref<VertexBuffer> VBO;
		Ref<IndexBuffer> IBO;

		int EntityID = -1;

		//temporary for forward draw
		bool isFirst = false;

		void SetupMesh(int entityID);
	};
}
