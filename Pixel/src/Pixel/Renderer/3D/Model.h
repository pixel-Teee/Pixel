#pragma once

#include "Pixel/Core/Core.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Pixel/Renderer/Shader.h"
#include "StaticMesh.h"

namespace Pixel {

	class Model
	{
	public:
		Model() = default;
		Model(const std::string& path)
		{
			LoadModel(path);
		}

		void Draw(const glm::mat4& transform, Ref<Shader>& shader, int entityID);

	private:

		std::vector<StaticMesh> m_Meshes;
		std::string m_directory;

		void LoadModel(const std::string& path);
		void ProcessNode(aiNode* node, const aiScene* scene);
		StaticMesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	};
}