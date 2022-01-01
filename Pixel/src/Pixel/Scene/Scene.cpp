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
		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto entity : group)
		{
			auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
			
			Renderer2D::DrawQuad(transform, sprite);
		}
	}

}

