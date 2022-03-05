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

		void Draw(const glm::mat4& transform, Ref<Shader>& shader, std::vector<Ref<Texture2D>> textures, int entityID);

		//TODO:forward temporary
		void Draw();
	private:
		//the model's every meshes
		std::vector<StaticMesh> m_Meshes;
		std::string m_directory;

		//load model and populate vertex information
		void LoadModel(const std::string& path);
		void ProcessNode(aiNode* node, const aiScene* scene);
		StaticMesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	};
}