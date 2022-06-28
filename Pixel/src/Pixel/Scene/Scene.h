#pragma once

#include "entt.hpp"
#include "Pixel/Core/UUID.h"
#include "Pixel/Core/Timestep.h"
#include "Pixel/Renderer/EditorCamera.h"
#include "Pixel/Renderer/Framebuffer.h"
#include "Pixel/Renderer/Texture.h"
#include "Pixel/Renderer/3D/GeometryPass.h"

class b2World;

class btBroadphaseInterface;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;

namespace Pixel
{
	class Entity;
	class PhysicsDraw;
	class Scene
	{
	public:
		Scene();
		~Scene();

		static Ref<Scene> Copy(Ref<Scene> other);

		Entity CreateEntity(const std::string& name);
		Entity CreateEntityWithUUID(UUID uuid, const std::string& name);
		void DestroyEntity(Entity entity);

		void OnRuntimeStart();
		void OnRuntimeStop();

		entt::registry& Reg() { return m_Registry; }

		void OnUpdateEditorForward(Timestep& ts, EditorCamera& camera, Ref<Framebuffer>& pFrameBuffer);
		void OnUpdateEditorDeferred(Timestep& ts, EditorCamera& camera, Ref<Framebuffer>& pFrameBuffer);
		//TODO:need to refractor
		void OnUpdateEditor(Timestep& ts, EditorCamera& camera, Ref<Framebuffer>& m_GeoPassFramebuffer, Ref<Framebuffer>& m_LightPassFramebuffer);
		void OnUpdateRuntime(Timestep& ts, Ref<Framebuffer>& m_GeoPassFramebuffer, Ref<Framebuffer>& m_LightPassFramebuffer);
		void OnViewportResize(uint32_t width, uint32_t height);

		Entity DuplicateEntity(Entity entity);

		Entity GetPrimaryCameraEntity();

		void SetViewPortSize(int m_ViewportWidth, int m_ViewportHeight);
		glm::vec2 GetViewPortSize();

		void SetSkyBox(Ref<CubeMap> skyBox);

		void MarkMeshEntityIDDirty();

		//Get Registry
		entt::registry& GetRegistry() { return m_Registry;  }

		btDiscreteDynamicsWorld* GetPhysicalWorld() { return m_world; }
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
	private:
		//environment
		Ref<CubeMap> m_skyBox;

		//geometry pass
		Ref<GeometryPass> m_pGeometryPass;

		//registry is a container for entity and component
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		//b2World* m_PhysicsWorld = nullptr;

		//-----Physical------
		btBroadphaseInterface* m_BroadPhase = nullptr;
		btDefaultCollisionConfiguration* m_collisionConfiguration = nullptr;
		btCollisionDispatcher* m_dispatcher = nullptr;
		btSequentialImpulseConstraintSolver* m_solver = nullptr;
		btDiscreteDynamicsWorld* m_world = nullptr;
		PhysicsDraw* m_debugDraw = nullptr;
		//-----Physical------
		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};

	

}