#pragma once

#include "BaseType.h"

#include "Pixel/Renderer/VertexArray.h"
#include "Pixel/Renderer/Buffer.h"
#include "Pixel/Renderer/Shader.h"
#include "Pixel/Renderer/Texture.h"
#include "Pixel/Renderer/UniformBuffer.h"
#include "Pixel/Renderer/RendererType.h"

namespace Pixel {

	struct alignas(256) MeshConstant
	{
		glm::mat4 world;
		glm::mat4 invWorld;
		glm::mat4 previousWorld;//use for TAA
		int editor;
	};

	class MaterialInstance;
	class Context;
	struct MaterialComponent;
	class DescriptorHeap;
	struct SubMaterial;
	class Material;
	class Bone;
	class StaticMesh
	{
	public:
		StaticMesh();
		StaticMesh(const StaticMesh& others);
		StaticMesh& operator=(const StaticMesh& rhs);
		~StaticMesh();

		StaticMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t> indices);

		void Draw(const glm::mat4& transform, Ref<Shader>& shader, std::vector<Ref<Texture2D>> textures, int entityID, Ref<UniformBuffer> modelUniformBuffer, bool bEntityDirty);

		//TODO:
		void Draw(const glm::mat4& transform, Ref<MaterialInstance> pMaterialInstance, int entityID);
		//TODO:temporary forward draw
		void Draw();

		//TODO:Test
		void Draw(Ref<Context> pContext, const glm::mat4& transform, int32_t entityId);

		//TODO:test for material tree
		void Draw(Ref<Context> pContext, const glm::mat4& transform, int32_t entityId, Ref<SubMaterial> pMaterial, Ref<Material> pTestMaterial, Ref<Shader> pVertexShader, Ref<Shader> pPixelShader);

		void Draw(Ref<Context> pContext, const glm::mat4& transform, int32_t entityId, Ref<SubMaterial> pMaterial, Ref<Shader> pVertexShader, Ref<Shader> pPixelShader);

		//TODO:test for material tree
		void Draw(Ref<Context> pContext, const glm::mat4& transform, int32_t entityId, Ref<MaterialInstance> pMaterialInstance, Ref<CbufferGeometryPass> geometryPass);

		void DrawShadowMap(Ref<Context> pContext, Ref<Shader> pShader, const glm::mat4& transform, int32_t entityId);
		void DrawOutLine(Ref<Context> pContext, const glm::mat4& transform);

		//Ref<VertexArray> GetVerterArray();
		Ref<VertexBuffer> GetVertexBuffer();

		unsigned char* GetIndexBuffer() { return m_Index; }
		unsigned char* GetDataBuffer(Semantics channel) { return m_DataBuffer[(uint64_t)channel]; }
		uint32_t GetDataBufferSize(Semantics channel) { return m_DataBufferSize[(uint64_t)channel]; }
		uint32_t GetIndexBufferSize() { return m_IndexSize; }

		std::string& GetName() { return m_Name; }
	private:
		/*------Data------*/
		unsigned char* m_DataBuffer[(uint64_t)Semantics::MAX];
		uint32_t m_DataBufferSize[(uint64_t)Semantics::MAX];
		unsigned char* m_Index = nullptr;
		uint32_t m_IndexSize = 0;
		unsigned char* m_AlternationDataBuffer = nullptr;
		uint32_t m_AlternationDataBufferSize = 0;//total databuffer size
		/*------Data------*/

		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;

		std::vector<Ref<Bone>> m_AffectBones;
		std::vector<glm::mat4> m_FinalMatrices;

		std::string m_Name;//mesh name

		int32_t PsoIndex = -1;//pipeline state object index, one for opaque, one for transparent
		uint32_t TransParentPsoIndex;

		bool m_IsFirstCreateMaterialPso = false;

		int EntityID = -1;

		//temporary for forward draw
		bool isFirst = false;

		void SetupMesh(int entityID, bool bHavedSwitched);

		MeshConstant m_MeshConstant;

		MaterialConstant m_MaterialConstant;

		friend class Model;
		friend struct StaticMeshComponent;
	};
}
