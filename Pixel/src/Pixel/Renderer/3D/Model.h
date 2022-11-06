#pragma once

#include "Pixel/Core/Core.h"
#include "Pixel/Renderer/Shader.h"
#include "StaticMesh.h"
#include "Pixel/Animation/BoneInfo.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Pixel {
	class Skeleton;
	class Bone;
	class MaterialTreeComponent;//material tree component
	class Model
	{
	public:
		Model() = default;
		Model(const std::string& path)
		{
			LoadModel(path);
		}

		Model(const std::string& path, MaterialTreeComponent& materialTreeComponent)
		{
			LoadModel(path, materialTreeComponent);
		}

		void Draw(const glm::mat4& transform, Ref<Shader>& shader, std::vector<Ref<Texture2D>> textures, int entityID, Ref<UniformBuffer> modelUniformBuffer);
		void Draw(const glm::mat4& transform, Ref<MaterialInstance> pMaterialInstance, int entityID);
		//TODO:forward temporary
		void Draw();

		void Draw(const glm::mat4& transform, Ref<Context> pContext, int32_t entityId);
		void DrawShadowMap(const glm::mat4& transform, Ref<Context> pContext, Ref<Shader> pShader, int32_t entityId);
		void Draw(const glm::mat4& transform, Ref<Context> pContext, int32_t entityId, MaterialComponent* pMaterialCompoent, Ref<Shader> pVertexShader, Ref<Shader> pPixelShader);
		void DrawOutLine(const glm::mat4& transform, Ref<Context> pContext);

		std::vector<Ref<StaticMesh>>& GetMeshes() { return m_Meshes; }

		void SetEntityDirty(bool dirty) { m_EntityDirty = dirty; }
		bool m_EntityDirty = false;

		static Ref<Model> Create(const std::string& path);

		static Ref<Model> Create(const std::string& path, MaterialTreeComponent& materialTreeComponent);

		//read heirarchy data, to construct bone 
		void ReadHeirarchyData(const aiNode* src, Ref<Bone> parentBone);

	private:
		//the model's every meshes
		std::vector<Ref<StaticMesh>> m_Meshes;
		std::string m_directory;

		std::string m_ModelPath;
		uint32_t m_MaterialIndex;//temp for material name

		std::map<std::string, BoneInfo> m_BoneInfoMap;
		int32_t m_BoneCounter = 0;

		Ref<Skeleton> m_pSkeleton;

		auto& GetBoneInfoMap() { return m_BoneInfoMap; }
		int32_t GetBoneCount() { return m_BoneCounter; }

		//load model and populate vertex information
		void LoadModel(const std::string& path);

		void LoadModel(const std::string& path, MaterialTreeComponent& materialTreeComponent);

		void ProcessNode(aiNode* node, const aiScene* scene);
		void ProcessNode(aiNode* node, const aiScene* scene, MaterialTreeComponent& materialTreeComponent);
		Ref<StaticMesh> ProcessMesh(aiMesh* mesh, const aiScene* scene);
		Ref<StaticMesh> ProcessMesh(aiMesh* mesh, const aiScene* scene, MaterialTreeComponent& materialTreeComponent);

		friend class BaseRenderer;
		friend class Animation;

		RTTR_ENABLE()
		RTTR_REGISTRATION_FRIEND

		friend struct StaticMeshComponent;
	};
}