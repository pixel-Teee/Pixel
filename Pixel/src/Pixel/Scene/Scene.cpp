#include "pxpch.h"
#include "Scene.h"

#include "Components.h"
#include "ScriptableEntity.h"
#include "Pixel/Renderer/Renderer2D.h"
#include "Pixel/Renderer/3D/Renderer3D.h"

#include <glm/glm.hpp>

//Box2D Stuff
#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"

namespace Pixel
{
	static b2BodyType Rigidbody2DTypeToBox2DType(Rigidbody2DComponent::BodyType bodyType)
	{
		switch (bodyType)
		{
			case Rigidbody2DComponent::BodyType::Static: return b2BodyType::b2_staticBody;
			case Rigidbody2DComponent::BodyType::Dynamic: return b2BodyType::b2_dynamicBody;
			case Rigidbody2DComponent::BodyType::Kinematic: return b2BodyType::b2_kinematicBody;
		}

		PX_CORE_ASSERT(false, "Unknown body type");
		return b2_staticBody;
	}

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

		//m_Registry.on_construct<CameraComponent>().connect<&Function>();
	}

	Scene::~Scene()
	{

	}

	template<typename Component>
	static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		auto view = src.view<Component>();
		for (auto e : view)
		{		
			UUID uuid = src.get<IDComponent>(e).ID;
			PX_CORE_ASSERT(enttMap.find(uuid) != enttMap.end(), "Not Find uuid!");
			entt::entity dstEnttID = enttMap.at(uuid);

			auto& component = src.get<Component>(e);
			dst.emplace_or_replace<Component>(dstEnttID, component);
		}
	}

	template<typename Component>
	static void CopyComponentIfExists(Entity dst, Entity src)
	{
		if (src.HasComponent<Component>())
		{
			dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
		}
	}

	Ref<Scene> Scene::Copy(Ref<Scene> other)
	{
		Ref<Scene> newScene = CreateRef<Scene>();

		newScene->m_ViewportWidth = other->m_ViewportWidth;
		newScene->m_ViewportHeight = other->m_ViewportHeight;

		std::unordered_map<UUID, entt::entity> enttMap;

		auto& srcSceneRegistry = other->m_Registry;
		auto& dstSceneRegistry = newScene->m_Registry;
		auto idView = srcSceneRegistry.view<IDComponent>();

		//Create entities in new scene
		for (auto e : idView)
		{
			UUID uuid = srcSceneRegistry.get<IDComponent>(e).ID;
			const auto& name = srcSceneRegistry.get<TagComponent>(e).Tag;
			Entity newEntity = newScene->CreateEntityWithUUID(uuid, name);
			enttMap[uuid] = (entt::entity)newEntity;
		}

		//Copy components(excepte IDComponent and TagComponent)
		CopyComponent<TransformComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<SpriteRendererComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<CameraComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<Rigidbody2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<BoxCollider2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<NativeScriptComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<MaterialComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<StaticMeshComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<LightComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);

		return newScene;
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		return CreateEntityWithUUID(UUID(), name);
	}

	Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<IDComponent>(uuid);
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>(name);
		tag.Tag = name.empty() ? "Entity" : name;

		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

	void Scene::OnRuntimeStart()
	{
		////Parameter:gravity
		//m_PhysicsWorld = new b2World({0.0f, -9.8f});
		//auto view = m_Registry.view<Rigidbody2DComponent>();

		//for (auto e : view)
		//{
		//	Entity entity = {e, this};
		//	auto& transform = entity.GetComponent<TransformComponent>();
		//	auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

		//	b2BodyDef bodyDef;
		//	bodyDef.type = Rigidbody2DTypeToBox2DType(rb2d.Type);
		//	bodyDef.position.Set(transform.Translation.x, transform.Translation.y);
		//	bodyDef.angle = transform.Rotation.z;
		//	b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
		//	body->SetFixedRotation(rb2d.FixedRotation);
		//	rb2d.RuntimeBody = body;

		//	if (entity.HasComponent<BoxCollider2DComponent>())
		//	{
		//		auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();

		//		b2PolygonShape boxShape;
		//		boxShape.SetAsBox(transform.Scale.x * bc2d.Size.x, transform.Scale.y * bc2d.Size.y);

		//		b2FixtureDef fixtureDef;
		//		fixtureDef.shape = &boxShape;
		//		fixtureDef.density = bc2d.Density;
		//		fixtureDef.friction = bc2d.Friction;
		//		fixtureDef.restitution = bc2d.Restitution;
		//		fixtureDef.restitutionThreshold = bc2d.RestitutionThreshold;
		//		body->CreateFixture(&fixtureDef);
		//	}
		//}
	}

	void Scene::OnRuntimeStop()
	{
		/*delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;*/
	}

	void Scene::OnUpdateEditor(Timestep& ts, EditorCamera& camera, Ref<Framebuffer>& m_GeoPassFramebuffer, Ref<Framebuffer>& m_LightPassFramebuffer)
	{
		Renderer3D::BeginScene(camera, m_GeoPassFramebuffer);

		auto group = m_Registry.group<TransformComponent>(entt::get<StaticMeshComponent, MaterialComponent>);
		
		for (auto entity : group)
		{
			auto [transform, material, mesh] = group.get<TransformComponent, MaterialComponent, StaticMeshComponent>(entity);

			//auto material = group.<MaterialComponent>(entity);
			Renderer3D::DrawModel(transform.GetTransform(), mesh, material, (int)entity);
		}

		//TODO:Point Light
		std::vector<TransformComponent> Trans;
		std::vector<LightComponent> Lights;
		auto group2 = m_Registry.view<TransformComponent, LightComponent>();

		for (auto entity : group2)
		{
			auto [transform, point] = group2.get<TransformComponent, LightComponent>(entity);
			transform.SetScale(glm::vec3(point.GetSphereLightVolumeRadius()));
			Lights.push_back(point);
			Trans.push_back(transform);
			//Trans.back().SetScale(glm::vec3(Lights.back().GetSphereLightVolumeRadius()));
		}
		
		Renderer3D::EndScene(camera, glm::vec2(m_ViewportWidth, m_ViewportHeight), Trans, Lights, m_GeoPassFramebuffer, m_LightPassFramebuffer);

		Renderer3D::DrawSkyBox(camera, m_LightPassFramebuffer, m_GeoPassFramebuffer);
	}

	void Scene::OnUpdateRuntime(Timestep& ts, Ref<Framebuffer>& m_GeoPassFramebuffer, Ref<Framebuffer>& m_LightPassFramebuffer)
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

		////Physics
		//{
		//	const int32_t velocityIterations = 6;
		//	const int32_t positionIterations = 2;
		//	m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);

		//	//Retrive transform from box2d
		//	auto view = m_Registry.view<Rigidbody2DComponent>();
		//	for (auto e : view)
		//	{
		//		Entity entity = {e, this};
		//		auto& transform = entity.GetComponent<TransformComponent>();
		//		auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

		//		b2Body* body = (b2Body*)rb2d.RuntimeBody;
		//		const auto& position = body->GetPosition();
		//		transform.Translation.x = position.x;
		//		transform.Translation.y = position.y;
		//		transform.Rotation.z = body->GetAngle();
		//	}
		//}

		//Camera* mainCamera = nullptr;
		//glm::mat4* cameraTransform;
		//{
		//	//Render 2D
		//	auto view = m_Registry.view<TransformComponent, CameraComponent>();
		//	for (auto entity : view)
		//	{
		//		auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

		//		if (camera.Primary)
		//		{
		//			mainCamera = &camera.camera;
		//			cameraTransform = &transform.GetTransform();
		//			break;
		//		}
		//	}
		//}
		//
		//if (mainCamera)
		//{
		//	Renderer2D::BeginScene(*mainCamera, *cameraTransform);

		//	auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		//	for (auto entity : group)
		//	{
		//		auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

		//		//Renderer2D::DrawQuad(transform.GetTransform(), sprite);
		//		//Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
		//	}

		//	Renderer2D::EndScene();
		//}		

		Camera* mainCamera = nullptr;
		TransformComponent* cameraTransform;
		{
			//Renderer 3D
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

				if (camera.Primary)
				{
					mainCamera = &camera.camera;
					cameraTransform = &transform;
					break;
				}
			}
		}

		if (mainCamera)
		{
			Renderer3D::BeginScene(*mainCamera, *cameraTransform, m_GeoPassFramebuffer);
			auto group = m_Registry.group<TransformComponent>(entt::get<StaticMeshComponent, MaterialComponent>);

			for (auto entity : group)
			{
				auto [transform, material, mesh] = group.get<TransformComponent, MaterialComponent, StaticMeshComponent>(entity);

				//auto material = group.<MaterialComponent>(entity);
				Renderer3D::DrawModel(transform.GetTransform(), mesh, material, (int)entity);
			}

			//TODO:Point Light
			std::vector<TransformComponent> Trans;
			std::vector<LightComponent> Lights;
			auto group2 = m_Registry.view<TransformComponent, LightComponent>();

			for (auto entity : group2)
			{
				auto [transform, point] = group2.get<TransformComponent, LightComponent>(entity);
				transform.SetScale(glm::vec3(point.GetSphereLightVolumeRadius()));
				Lights.push_back(point);
				Trans.push_back(transform);
				//Trans.back().SetScale(glm::vec3(Lights.back().GetSphereLightVolumeRadius()));
			}

			Renderer3D::EndScene(*mainCamera, *cameraTransform, glm::vec2(m_ViewportWidth, m_ViewportHeight), Trans, Lights, m_GeoPassFramebuffer, m_LightPassFramebuffer);

			Renderer3D::DrawSkyBox(*mainCamera, *cameraTransform, m_LightPassFramebuffer, m_GeoPassFramebuffer);
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

	void Scene::DuplicateEntity(Entity entity)
	{
		//Copy Name
		std::string name = entity.GetName();
		Entity newEntity = CreateEntity(name);

		CopyComponentIfExists<TransformComponent>(newEntity, entity);
		CopyComponentIfExists<SpriteRendererComponent>(newEntity, entity);
		CopyComponentIfExists<CameraComponent>(newEntity, entity);
		CopyComponentIfExists<Rigidbody2DComponent>(newEntity, entity);
		CopyComponentIfExists<BoxCollider2DComponent>(newEntity, entity);
		CopyComponentIfExists<NativeScriptComponent>(newEntity, entity);
		CopyComponentIfExists<MaterialComponent>(newEntity, entity);
		CopyComponentIfExists<StaticMeshComponent>(newEntity, entity);
		CopyComponentIfExists<LightComponent>(newEntity, entity);
	}

	Entity Scene::GetPrimaryCameraEntity()
	{
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			const auto& camera = view.get<CameraComponent>(entity);
			if (camera.Primary)
				return Entity{ entity, this };
		}
		return {};
	}

	void Scene::SetViewPortSize(int width, int height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;
	}

	glm::vec2 Scene::GetViewPortSize()
	{
		return {m_ViewportWidth, m_ViewportHeight};
	}

	template<typename T>
	void Pixel::Scene::OnComponentAdded(Entity entity, T& component)
	{
		//static_assert(false);
	}

	template<>
	void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		component.camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}

	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{
		
	}

	template<>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
	{
		
	}

	template<>
	void Scene::OnComponentAdded<StaticMeshComponent>(Entity entity, StaticMeshComponent& component)
	{
		component.mesh = Model(component.path);
	}

	template<>
	void Scene::OnComponentAdded<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<MaterialComponent>(Entity entity, MaterialComponent& component)
	{
		
	}

	template<>
	void Scene::OnComponentAdded<LightComponent>(Entity entity, LightComponent& component)
	{

	}
}

