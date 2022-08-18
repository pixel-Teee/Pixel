#pragma once

#include "Pixel/Renderer/EditorCamera.h"
#include "Pixel/Renderer/Framebuffer.h"
#include "Pixel/Scene/Components.h"
#include <string>

namespace Pixel{
	class Renderer3D
	{
	public:
		static void Init();

		//------Draw Model------
		static void DrawModel(const glm::mat4& transform, StaticMeshComponent& MeshComponent, MaterialComponent& Material, int EntityID);
		
		static void BeginScene(const Camera& camera, TransformComponent& trans, Ref<Framebuffer> geometryFramebuffer);
		static void BeginScene(const EditorCamera& camera, Ref<Framebuffer> geometryFramebuffer);

		static void EndScene(const EditorCamera& camera, glm::vec2& gScreenSize, std::vector<TransformComponent>& Trans, std::vector<LightComponent>& Lights, Ref<Framebuffer> geoPassFramebuffer, Ref<Framebuffer> LightPassFramebuffer);
		static void EndScene(Camera& camera, TransformComponent& trans, glm::vec2& gScreenSize, std::vector<TransformComponent>& Trans, std::vector<LightComponent>& Lights, Ref<Framebuffer> geoPassFramebuffer, Ref<Framebuffer> LightPassFramebuffer);
		//------Draw Model------

		//------give editor use------
		static Ref<CubeMap> GetSkyBox();
		static Ref<CubeMap> GetDefaultSkyBox();

		static void DrawSkyBox(const EditorCamera& camera, Ref<Framebuffer> LightPassFramebuffer, Ref<Framebuffer> geoPassFramebuffer);

		static void DrawSkyBox(Camera& camera, TransformComponent& trans, Ref<Framebuffer> LightPassFramebuffer, Ref<Framebuffer> geoPassFramebuffer);

		static void DrawLine(const glm::vec3& from, const glm::vec3& to, const glm::vec3 color, glm::mat4 viewProjection);
		//-------shader generator string-------
	};

}