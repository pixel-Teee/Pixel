#pragma once

#include "Pixel/Core/Core.h"
#include "Pixel/Renderer/Shader.h"
#include "StaticMesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Pixel {

	class Model
	{
	public:
		Model() = default;
		Model(const std::string& path)
		{
			LoadModel(path);
		}

		void Draw(const glm::mat4& transform, Ref<Shader>& shader, std::vector<Ref<Texture2D>> textures, int entityID, Ref<UniformBuffer> modelUniformBuffer);
		void Draw(const glm::mat4& transform, Ref<MaterialInstance> pMaterialInstance, int entityID);
		//TODO:forward temporary
		void Draw();

		void Draw(const glm::mat4& transform, Ref<Context> pContext, int32_t entityId);
		void DrawShadowMap(const glm::mat4& transform, Ref<Context> pContext, int32_t entityId);
		void Draw(const glm::mat4& transform, Ref<Context> pContext, int32_t entityId, MaterialComponent* pMaterialCompoent);
		void DrawOutLine(const glm::mat4& transform, Ref<Context> pContext);

		std::vector<Ref<StaticMesh>> GetMeshes() { return m_Meshes; }

		void SetEntityDirty(bool dirty) { m_EntityDirty = dirty; }
		bool m_EntityDirty = false;

		static Ref<Model> Create(const std::string& path);
	private:
		//the model's every meshes
		std::vector<Ref<StaticMesh>> m_Meshes;
		std::string m_directory;

		//load model and populate vertex information
		void LoadModel(const std::string& path);
		void ProcessNode(aiNode* node, const aiScene* scene);
		Ref<StaticMesh> ProcessMesh(aiMesh* mesh, const aiScene* scene);

		friend class BaseRenderer;

		RTTR_ENABLE()
		RTTR_REGISTRATION_FRIEND
	};
}