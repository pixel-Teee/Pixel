#include "pxpch.h"
#include "Scene.h"

#include "Components.h"
#include "Entity.h"
#include "Pixel/Renderer/Renderer2D.h"

#include <glm/glm.hpp>

namespace Pixel
{

	Scene::Scene()
	{
		/*
		entt::entity entity = m_Registry.create();
		TransformComponent transform;

		//transform = m_Registry.emplace<TransformComponent>(entity, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

		//transform = glm::mat4(1.0f);

		//TransformComponent& transform = m_Registry.get<TransformComponent>(entity);

		auto view = m_Registry.view<TransformComponent>();
		for (auto entity : view)
		{
			TransformComponent& transform = m_Registry.get<TransformComponent>(entity);
		}

		auto group = m_Registry.group<TransformComponent>(entt::get<MeshComponent>);

		for (auto entity : group)
		{
			auto& [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);
		}
		*/
	}

	Scene::~Scene()
	{

	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = {m_Registry.create(), this};
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>(name);
		tag.Tag = name.empty() ? "Entity" : name;
	
		return entity;
	}

	void Scene::OnUpdate(Timestep& ts)
	{
		//Update Scripts	
		{
				m_Registry.view<NativeScriptComponent>().each(
				[=](auto entity, auto& nsc)
				{
					//ToDo: Move to Scene:OnScenePlay
					if (!nsc.Instance)
					{
						nsc.Instance = nsc.InstantiateScript();
						nsc.Instance->m_Entity = Entity{entity, this};
						nsc.Instance->OnCreate();
					}

					nsc.Instance->OnUpdate(ts);
				});
		}

		Camera* mainCamera = nullptr;
		glm::mat4* cameraTransform;
		{
			//Render 2D
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

				if (camera.Primary)
				{
					mainCamera = &camera.camera;
					cameraTransform = &transform.GetTransform();
					break;
				}
			}
		}

		if (mainCamera)
		{
			Renderer2D::BeginScene(*mainCamera, *cameraTransform);

			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

				Renderer2D::DrawQuad(transform.GetTransform(), sprite);
			}

			Renderer2D::EndScene();
		}		
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		//Resize our non FixedAspectRatio Cameras
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.FixedAspectRatio)
			{
				cameraComponent.camera.SetViewportSize(width, height);
			}
		}
	}

}

