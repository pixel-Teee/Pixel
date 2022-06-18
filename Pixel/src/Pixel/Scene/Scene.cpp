#include "pxpch.h"
#include "Scene.h"

#include "Pixel/Core/Application.h"
#include "Components.h"
#include "ScriptableEntity.h"
#include "Pixel/Core/Input.h"
#include "Pixel/Renderer/Renderer2D.h"
#include "Pixel/Renderer/3D/Renderer3D.h"
#include "Pixel/Renderer/Context/Context.h"
#include "Pixel/Renderer/Context/ContextManager.h"
#include "Pixel/Renderer/Device/Device.h"
#include "Pixel/Core/Application.h"
#include "Pixel/Renderer/BaseRenderer.h"

#include <glm/glm.hpp>

//Box2D Stuff
#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"

#include "btBulletDynamicsCommon.h"
#include "Pixel/Physics/PhysicsDraw.h"

namespace Pixel
{
	Scene* g_Scene;

	struct CollCallBack : public btCollisionWorld::ContactResultCallback
	{
	public:
		btScalar addSingleResult(
			btManifoldPoint& cp,
			const btCollisionObjectWrapper* colObj0Wrap,
			int partId0,
			int index0,
			const btCollisionObjectWrapper* colObj1Wrap,
			int partId1,
			int index1)
		{
			/*btVector3 posA = cp.getPositionWorldOnA();
			btVector3 posB = cp.getPositionWorldOnB();
			printf("col pos for A {%f, %f, %f}\n", posA.getX(), posA.getY(), posA.getZ());
			printf("col pos for B {%f, %f, %f}\n", posB.getX(), posB.getY(), posB.getZ());*/

			PIXEL_CORE_INFO("×²µ½ÁË!");
			return btScalar(0.f);
		};
	};

	//------Lua------
	static int Lua_SetCursorViewPortCenter(lua_State* L)
	{
		Application::Get().GetWindow().SetCursorViewPortCenter();
		return 0;
	}

	static int Lua_SetCursorDisabled(lua_State* L)
	{
		Application::Get().GetWindow().SetCursorDisabled();
		return 0;
	}

	//------Lua------
	static int Lua_CopyEntity(lua_State* L)
	{
		const char* str = lua_tostring(ScriptableEntity::g_pLuaState, -1);

		entt::registry& Registry = g_Scene->GetRegistry();
		auto entities = Registry.view<TagComponent>();

		for (auto entity : entities)
		{
			TagComponent& tag = entities.get<TagComponent>(entity);
			if (tag.Tag == str)
			{
				Entity CopyedEntity = { entity, g_Scene };
				Entity newEntity = g_Scene->DuplicateEntity(CopyedEntity);
				lua_pushinteger(ScriptableEntity::g_pLuaState, uint32_t(newEntity));
				break;
			}
		}

		return 1;
	}

	static int Lua_GetEntityByName(lua_State* L)
	{
		const char* str = lua_tostring(ScriptableEntity::g_pLuaState, -1);
		entt::registry& Registry = g_Scene->GetRegistry();
		auto entities = Registry.view<TagComponent>();

		for (auto entity : entities)
		{
			TagComponent& tag = entities.get<TagComponent>(entity);
			if (tag.Tag == str)
			{
				//TODO: return entity or UUID
				lua_pushinteger(ScriptableEntity::g_pLuaState, (uint32_t)(entity));
				break;
			}
		}

		return 1;
	}

	static int Lua_SetTransformComponentLocation(lua_State* L)
	{
		int32_t entityId = lua_tointeger(ScriptableEntity::g_pLuaState, -4);

		Entity entity{ (entt::entity)entityId, g_Scene};
		TransformComponent& trans = entity.GetComponent<TransformComponent>();
		TagComponent& tag = entity.GetComponent<TagComponent>();
		PIXEL_CORE_INFO("{0}", tag.Tag);
		double x = lua_tonumber(ScriptableEntity::g_pLuaState, -3);
		double y = lua_tonumber(ScriptableEntity::g_pLuaState, -2);
		double z = lua_tonumber(ScriptableEntity::g_pLuaState, -1);

		trans.Translation = { x, y, z };

		//need to synchronization to phsyical world
		//have rg3d?

		if (entity.HasComponent<RigidBody3DComponent>())
		{
			RigidBody3DComponent& rg3d = entity.GetComponent<RigidBody3DComponent>();

			btTransform transform = ((btRigidBody*)rg3d.m_body)->getCenterOfMassTransform();
			transform.setOrigin(btVector3(x, y, z));
			//g_Scene->GetPhysicalWorld()->
			((btRigidBody*)rg3d.m_body)->setCenterOfMassTransform(transform);
		}

		//btTransform trans = ((btRigidBody*)rg3d.m_body)->getWorldTransform();

		return 0;
	}

	static int Lua_GetTransformComponentLocation(lua_State* L)
	{
		uint32_t entityId = lua_tointeger(ScriptableEntity::g_pLuaState, -1);

		Entity entity{ (entt::entity)entityId, g_Scene };
		TransformComponent& trans = entity.GetComponent<TransformComponent>();

		lua_pushnumber(ScriptableEntity::g_pLuaState, trans.Translation.x);
		lua_pushnumber(ScriptableEntity::g_pLuaState, trans.Translation.y);
		lua_pushnumber(ScriptableEntity::g_pLuaState, trans.Translation.z);

		return 3;
	}

	static int Lua_GetTransformComponentRotation(lua_State* L)
	{
		uint32_t entityId = lua_tointeger(ScriptableEntity::g_pLuaState, -1);

		Entity entity{ (entt::entity)entityId, g_Scene };
		TransformComponent& trans = entity.GetComponent<TransformComponent>();

		lua_pushnumber(ScriptableEntity::g_pLuaState, trans.Rotation.x);
		lua_pushnumber(ScriptableEntity::g_pLuaState, trans.Rotation.y);
		lua_pushnumber(ScriptableEntity::g_pLuaState, trans.Rotation.z);

		return 3;
	}

	static int Lua_SetTransformComponentRotation(lua_State* L)
	{
		uint32_t entityId = lua_tointeger(ScriptableEntity::g_pLuaState, -4);

		Entity entity{ (entt::entity)entityId, g_Scene };
		TransformComponent& trans = entity.GetComponent<TransformComponent>();

		double x = lua_tonumber(ScriptableEntity::g_pLuaState, -3);
		double y = lua_tonumber(ScriptableEntity::g_pLuaState, -2);
		double z = lua_tonumber(ScriptableEntity::g_pLuaState, -1);

		trans.Rotation = { x, y, z };

		return 0;
	}

	static int Lua_GetAsyncKeyInput(lua_State* L)
	{
		int32_t KeyCodes = lua_tointeger(ScriptableEntity::g_pLuaState, -1);
		if (Input::IsKeyPressed(KeyCodes))
		{
			lua_pushboolean(ScriptableEntity::g_pLuaState, true);
		}
		else
		{
			lua_pushboolean(ScriptableEntity::g_pLuaState, false);
		}

		return 1;
	}

	static int Lua_GetMousePos(lua_State* L)
	{
		std::pair<float, float> MousePos = Input::GetMousePosition();

		lua_pushnumber(ScriptableEntity::g_pLuaState, MousePos.first);
		lua_pushnumber(ScriptableEntity::g_pLuaState, MousePos.second);

		return 2;
	}

	static int Lua_GetTransformComponentDirectionAndRight(lua_State* L)
	{
		uint32_t entityId = lua_tointeger(ScriptableEntity::g_pLuaState, -1);

		Entity entity{ (entt::entity)entityId, g_Scene };
		TransformComponent& trans = entity.GetComponent<TransformComponent>();

		glm::vec3 front;
		front.x = -glm::cos(trans.Rotation.x) * glm::sin(trans.Rotation.y);
		front.y = glm::sin(-trans.Rotation.x);
		front.z = -glm::cos(-trans.Rotation.x) * glm::cos(trans.Rotation.y);
		front = glm::normalize(front);

		glm::vec3 Right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0)));

		lua_pushnumber(ScriptableEntity::g_pLuaState, front.x);
		lua_pushnumber(ScriptableEntity::g_pLuaState, front.y);
		lua_pushnumber(ScriptableEntity::g_pLuaState, front.z);
		lua_pushnumber(ScriptableEntity::g_pLuaState, Right.x);
		lua_pushnumber(ScriptableEntity::g_pLuaState, Right.y);
		lua_pushnumber(ScriptableEntity::g_pLuaState, Right.z);
		return 6;
	}

	static int Lua_SetLightComponentColor(lua_State* L)
	{
		uint32_t entityId = lua_tonumber(ScriptableEntity::g_pLuaState, -4);

		Entity entity{ (entt::entity)entityId, g_Scene };
		LightComponent& light = entity.GetComponent<LightComponent>();

		double x = lua_tonumber(ScriptableEntity::g_pLuaState, -3);
		double y = lua_tonumber(ScriptableEntity::g_pLuaState, -2);
		double z = lua_tonumber(ScriptableEntity::g_pLuaState, -1);
		
		light.color = { x, y, z };
		return 0;
	}
	//------Lua------

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

	static btConvexHullShape* ConstructConvex(const std::vector<StaticMesh>& meshes, glm::mat4& transform)
	{
		btConvexHullShape* Convex = new btConvexHullShape();
		int32_t i = 0;
		
		for (auto mesh : meshes)
		{
			float x = 0.0f;
			float y = 0.0f;
			float z = 0.0f;

			unsigned char* data = mesh.GetDataBuffer(Semantics::POSITION);
			uint32_t size = mesh.GetDataBufferSize(Semantics::POSITION);
			for (size_t j = 0; j < size; j += 12)
			{
				memcpy(&x, data + j, 4);
				memcpy(&y, data + j + 4, 4);
				memcpy(&z, data + j + 8, 4);
				glm::vec4 pos =  glm::vec4(x, y, z, 1.0f);
				btVector3 btv = btVector3(pos.x, pos.y, pos.z);
				Convex->addPoint(btv);
			}
		}
		return Convex;
	}

	static btBvhTriangleMeshShape* ConstructTriangleMesh(const std::vector<StaticMesh>& meshes, glm::mat4& transform)
	{
		btTriangleMesh* TringleMesh = new btTriangleMesh();
		for (auto mesh : meshes)
		{
			float x = 0.0f;
			float y = 0.0f;
			float z = 0.0f;

			unsigned char* data = mesh.GetDataBuffer(Semantics::POSITION);
			unsigned char* index = mesh.GetIndexBuffer();
			uint32_t size = mesh.GetDataBufferSize(Semantics::POSITION);
			uint32_t indexSize = mesh.GetIndexBufferSize();
			for (uint32_t i = 0; i < indexSize; i += 12)
			{
				uint32_t id;
				memcpy(&id, &index[i], 4);

				float x = 0.0f;
				float y = 0.0f;
				float z = 0.0f;
				memcpy(&x, &data[id * 12], 4);
				memcpy(&y, &data[id * 12 + 4], 4);
				memcpy(&z, &data[id * 12 + 8], 4);

				float v1x = x, v1y = y, v1z = z;

				memcpy(&id, &index[i + 4], 4);

				memcpy(&x, &data[id * 12], 4);
				memcpy(&y, &data[id * 12 + 4], 4);
				memcpy(&z, &data[id * 12 + 8], 4);

				float v2x = x, v2y = y, v2z = z;

				memcpy(&id, &index[i + 8], 4);

				memcpy(&x, &data[id * 12], 4);
				memcpy(&y, &data[id * 12 + 4], 4);
				memcpy(&z, &data[id * 12 + 8], 4);

				float v3x = x, v3y = y, v3z = z;

				glm::vec4 pos1 = glm::vec4(v1x, v1y, v1z, 1.0f);
				glm::vec4 pos2 = glm::vec4(v2x, v2y, v2z, 1.0f);
				glm::vec4 pos3 = glm::vec4(v3x, v3y, v3z, 1.0f);

				btVector3 bv1 = btVector3(pos1.x, pos1.y, pos1.z);
				btVector3 bv2 = btVector3(pos2.x, pos2.y, pos2.z);
				btVector3 bv3 = btVector3(pos3.x, pos3.y, pos3.z);

				TringleMesh->addTriangle(bv1, bv2, bv3);
			}
		}

		return new btBvhTriangleMeshShape(TringleMesh, true);
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

		//Create Geometry Pass
		//m_pGeometryPass = CreateRef<GeometryPass>();
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
		//newScene->m_pGeometryPass = other->m_pGeometryPass;

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
		CopyComponent<MaterialTreeComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<RigidBody3DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);

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
		//Application::Get().GetWindow().SetCursorPos(0, 0);
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

		//------Physical------
		m_collisionConfiguration = new btDefaultCollisionConfiguration();
		m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
		m_BroadPhase = new btDbvtBroadphase();
		m_solver = new btSequentialImpulseConstraintSolver;
		m_world = new btDiscreteDynamicsWorld(m_dispatcher, m_BroadPhase, m_solver, m_collisionConfiguration);
		m_debugDraw = new PhysicsDraw();
		m_debugDraw->setDebugMode(btIDebugDraw::DebugDrawModes::DBG_DrawAabb);
		m_world->setDebugDrawer(m_debugDraw);

		//set gravity
		m_world->setGravity(btVector3(0, -9.8, 0));

		auto view = m_Registry.view<RigidBody3DComponent, StaticMeshComponent>();

		for (auto e : view)
		{
			Entity entity = { e, this };
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rg3d = entity.GetComponent<RigidBody3DComponent>();
			auto& model = entity.GetComponent<StaticMeshComponent>();

			btCollisionShape* Shape;

			if (rg3d.m_shapeType == RigidBody3DComponent::ShapeType::BoxShape)
			{
				Shape = new btBoxShape(btVector3(rg3d.m_HalfLength, rg3d.m_HalfLength, rg3d.m_HalfLength));
			}
			else if (rg3d.m_shapeType == RigidBody3DComponent::ShapeType::SphereShape)
			{
				Shape = new btSphereShape(rg3d.m_Radius);
			}
			else
			{
				Shape = ConstructConvex(model.mesh.GetMeshes(), transform.GetTransform());
			}
			
			btQuaternion rotation;
			rotation.setEulerZYX(transform.Rotation.z, transform.Rotation.y, transform.Rotation.x);

			btVector3 position;
			position = btVector3(transform.Translation.x, transform.Translation.y, transform.Translation.z);

			btDefaultMotionState* montionState = new btDefaultMotionState(btTransform(rotation, position));

			btScalar bodyMass = rg3d.m_bodyMass;
			btVector3 bodyInertia = btVector3(rg3d.m_bodyInertia.x, rg3d.m_bodyInertia.y, rg3d.m_bodyInertia.z);

			//constuct infomation
			btRigidBody::btRigidBodyConstructionInfo bodyCI =
				btRigidBody::btRigidBodyConstructionInfo(bodyMass, montionState, Shape, bodyInertia);

			bodyCI.m_restitution = rg3d.m_restitution;
			bodyCI.m_friction = rg3d.m_friction;

			rg3d.m_body = new btRigidBody(bodyCI);

			((btRigidBody*)rg3d.m_body)->setLinearFactor(btVector3(1.0f, 1.0f, 1.0f));

			m_world->addRigidBody((btRigidBody*)rg3d.m_body);
		}
		//------Physical------
		
		//TODO:fix
		g_Scene = this;
		if (ScriptableEntity::g_pLuaState == nullptr)
		{
			//---Open Lua---
			ScriptableEntity::g_pLuaState = luaL_newstate();
			luaL_openlibs(ScriptableEntity::g_pLuaState);
			lua_register(ScriptableEntity::g_pLuaState, "Host_Print", Lua_Print);
			//---Open Lua---

			//---Create Global Function---
			//---Create Entity---
			lua_register(ScriptableEntity::g_pLuaState, "Host_GetEntityByName", Lua_GetEntityByName);
			lua_register(ScriptableEntity::g_pLuaState, "Host_SetTransformComponentLocation", Lua_SetTransformComponentLocation);
			lua_register(ScriptableEntity::g_pLuaState, "Host_GetTransformComponentLocation", Lua_GetTransformComponentLocation);
			lua_register(ScriptableEntity::g_pLuaState, "Host_GetAsyncKeyInput", Lua_GetAsyncKeyInput);
			lua_register(ScriptableEntity::g_pLuaState, "Host_SetTransformComponentRotation", Lua_SetTransformComponentRotation);
			lua_register(ScriptableEntity::g_pLuaState, "Host_GetTransformComponentRotation", Lua_GetTransformComponentRotation);
			lua_register(ScriptableEntity::g_pLuaState, "Host_GetMousePos", Lua_GetMousePos);
			lua_register(ScriptableEntity::g_pLuaState, "Host_GetTransformComponentDirectionAndRight", Lua_GetTransformComponentDirectionAndRight);
			lua_register(ScriptableEntity::g_pLuaState, "Host_SetLightComponentColor", Lua_SetLightComponentColor);
			lua_register(ScriptableEntity::g_pLuaState, "Host_SetCursorViewPortCenter", Lua_SetCursorViewPortCenter);
			lua_register(ScriptableEntity::g_pLuaState, "Host_SetCursorHidden", Lua_SetCursorDisabled);
			lua_register(ScriptableEntity::g_pLuaState, "Host_CopyEntity", Lua_CopyEntity);
			//---Create Entity---
			//---Create Global Function---
		}
	}

	void Scene::OnRuntimeStop()
	{
		/*m_Registry.view<RigidBody3DComponent>().each(
			[=](auto entity, auto& rg3d) {

			if (rg3d.m_body != nullptr)
			{
				btRigidBody* temp = (btRigidBody*)rg3d.m_body;
				delete temp->getMotionState();
				delete temp;
				rg3d.m_body = nullptr;
			}

			if (rg3d.m_shape != nullptr)
			{
				btConvexHullShape* temp = (btConvexHullShape*)rg3d.m_shape;
				delete temp;
				rg3d.m_shape = nullptr;
			}
		});*/

		delete m_debugDraw;
		m_debugDraw = nullptr;

		delete m_world;
		m_world = nullptr;

		/*delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;*/
		m_Registry.view<NativeScriptComponent>().each(
			[=](auto entity, auto& nsc)
		{
			//ToDo: Move to Scene:OnScenePlay
			if (nsc.Instance)
			{
				//nsc.Instantiate(nsc.m_path);
				//nsc.Instance->m_Entity = Entity{ entity, this };
				nsc.Instance->OnDestroy();
			}
		});

		if (ScriptableEntity::g_pLuaState != nullptr)
		{
			lua_close(ScriptableEntity::g_pLuaState);
			ScriptableEntity::g_pLuaState = nullptr;
		}

		//g_Scene = nullptr;
		g_Scene = nullptr;
	}

	void Scene::OnUpdateEditorForward(Timestep& ts, EditorCamera& camera, Ref<Framebuffer>& pFrameBuffer)
	{
		auto group = m_Registry.group<TransformComponent>(entt::get<StaticMeshComponent>);

		std::vector<TransformComponent> trans;
		std::vector<StaticMeshComponent> meshs;
		std::vector<int32_t> entityIds;
		for (auto entity : group)
		{
			auto [transform, mesh] = group.get<TransformComponent, StaticMeshComponent>(entity);

			//in terms of transform and mesh to draw
			trans.push_back(transform);
			meshs.push_back(mesh);
			entityIds.push_back((int32_t)entity);
		}

		std::vector<LightComponent> lights;
		std::vector<TransformComponent> lightTrans;
		auto lightGroup = m_Registry.group<LightComponent>(entt::get<TransformComponent>);
		for (auto entity : lightGroup)
		{
			auto [trans, light] = lightGroup.get<TransformComponent, LightComponent>(entity);

			//in terms of transform and mesh to draw
			lightTrans.push_back(trans);
			lights.push_back(light);
		}

		Ref<Context> pContext = Device::Get()->GetContextManager()->AllocateContext(CommandListType::Graphics);
		Application::Get().GetRenderer()->ForwardRendering(pContext, camera, trans, meshs, lights, lightTrans, pFrameBuffer, entityIds);
		pContext->Finish(true);

		Ref<Context> pComputeContext = Device::Get()->GetContextManager()->AllocateContext(CommandListType::Compute);
		Application::Get().GetRenderer()->RenderPickerBuffer(pComputeContext, pFrameBuffer);
		//pComputeContext->Finish(true);
	}

	void Scene::OnUpdateEditor(Timestep& ts, EditorCamera& camera, Ref<Framebuffer>& m_GeoPassFramebuffer, Ref<Framebuffer>& m_LightPassFramebuffer)
	{
		//Renderer3D::BeginScene(camera, m_GeoPassFramebuffer);

		auto group = m_Registry.group<TransformComponent>(entt::get<StaticMeshComponent, MaterialComponent>);
		
		for (auto entity : group)
		{
			auto [transform, material, mesh] = group.get<TransformComponent, MaterialComponent, StaticMeshComponent>(entity);

			//auto material = group.<MaterialComponent>(entity);
			//Renderer3D::DrawModel(transform.GetTransform(), mesh, material, (int)entity);
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

		//Renderer3D::EndScene(camera, glm::vec2(m_ViewportWidth, m_ViewportHeight), Trans, Lights, m_GeoPassFramebuffer, m_LightPassFramebuffer);

		//Renderer3D::DrawSkyBox(camera, m_LightPassFramebuffer, m_GeoPassFramebuffer);
	}

	void Scene::OnUpdateRuntime(Timestep& ts, Ref<Framebuffer>& m_GeoPassFramebuffer, Ref<Framebuffer>& m_LightPassFramebuffer)
	{
		//Update Scripts	
		//{
		//		m_Registry.view<NativeScriptComponent>().each(
		//		[=](auto entity, auto& nsc)
		//		{
		//			//ToDo: Move to Scene:OnScenePlay
		//			if (!nsc.m_pLuaState)
		//			{
		//				nsc.Instance = nsc.InstantiateScript();
		//				nsc.Instance->m_Entity = Entity{entity, this};
		//				nsc.Instance->OnCreate();
		//			}

		//			nsc.Instance->OnUpdate(ts);
		//		});
		//}

		{
			m_Registry.view<NativeScriptComponent>().each(
				[=](auto entity, auto& nsc)
			{
				//ToDo: Move to Scene:OnScenePlay
				if (!nsc.Instance)
				{
					nsc.Instantiate(nsc.m_path);
					nsc.Instance->m_Entity = Entity{ entity, this };
					nsc.Instance->OnCreate();
				}

				nsc.Instance->OnUpdate(ts);
			});
		}

		//Physics
		{
			m_world->stepSimulation(ts.GetSeconds());

			auto view = m_Registry.view<RigidBody3DComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };
				auto& transform = entity.GetComponent<TransformComponent>();
				auto& rg3d = entity.GetComponent<RigidBody3DComponent>();

				btTransform trans = ((btRigidBody*)rg3d.m_body)->getWorldTransform();

				transform.Translation = glm::vec3(trans.getOrigin().x(), trans.getOrigin().y(), trans.getOrigin().z());
				trans.getRotation().getEulerZYX(transform.Rotation.z, transform.Rotation.y, transform.Rotation.x);

				//CollCallBack callBack;
				//m_world->contactTest((btRigidBody*)rg3d.m_body, callBack);
			}
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
			//Renderer3D::BeginScene(*mainCamera, *cameraTransform, m_GeoPassFramebuffer);
			auto group = m_Registry.group<TransformComponent>(entt::get<StaticMeshComponent, MaterialComponent>);

			for (auto entity : group)
			{
				auto [transform, material, mesh] = group.get<TransformComponent, MaterialComponent, StaticMeshComponent>(entity);

				//auto material = group.<MaterialComponent>(entity);
				//Renderer3D::DrawModel(transform.GetTransform(), mesh, material, (int)entity);
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

			//Renderer3D::EndScene(*mainCamera, *cameraTransform, glm::vec2(m_ViewportWidth, m_ViewportHeight), Trans, Lights, m_GeoPassFramebuffer, m_LightPassFramebuffer);

			//Renderer3D::DrawSkyBox(*mainCamera, *cameraTransform, m_LightPassFramebuffer, m_GeoPassFramebuffer);

			/*m_LightPassFramebuffer->Bind();
			m_LightPassFramebuffer->SetColorAttachmentDraw(0);
			PhysicsDraw* debugDraw = (PhysicsDraw*)m_world->getDebugDrawer();
			glm::mat4 view = cameraTransform->GetTransform();
			glm::mat4 proj = mainCamera->GetProjection();
			glm::mat4 viewProj = proj * glm::inverse(view);
			debugDraw->SetMatrix(viewProj);
			m_world->debugDrawWorld();
			m_LightPassFramebuffer->UnBind();*/
		}

		
		//m_LightPassFramebuffer->UnBind();
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
				//m_pGeometryPass->Resize(width, height);
			}
		}
	}

	static std::string CreateNewName(std::string OriginName, Scene* scene)
	{
		auto& entities = scene->GetRegistry().view<TagComponent>();

		bool HaveBracket = false;
		size_t left_bracket_pos = OriginName.find("(");
		size_t right_bracket_pos = OriginName.find(")");

		if (left_bracket_pos < right_bracket_pos) {
			HaveBracket = true;
		}
		else {
			HaveBracket = false;
		}
		std::string left, right;
		if (HaveBracket) {
			left = OriginName.substr(0, left_bracket_pos + 1);
			right = OriginName.substr(right_bracket_pos);
		}
		else {
			left = OriginName;
		}

		int32_t maxIndex = -1;
		for (auto& entity : entities)
		{
			Entity e = { entity, scene };
			std::string name = e.GetComponent<TagComponent>().Tag;
			if (name.size() < OriginName.size()) continue;
			if (HaveBracket) {
				bool flag = true;
				int32_t pos1 = -1;
				uint32_t i;
				for (i = 0; i < left.size(); ++i) {
					if (name[i] != left[i]) flag = false;
				}
				if (!flag) continue;
				pos1 = i;
				int32_t pos2 = -1;
				for (i = 0; i < right.size(); ++i) {
					if (name[name.size() - right.size() + i] != right[i]) flag = false;
				}
				if (!flag) continue;
				pos2 = name.size() - right.size();
				if (pos1 != -1 && pos2 != -1 && pos1 <= pos2)
					maxIndex = std::max(maxIndex, std::stoi(name.substr(pos1, pos2 - pos1 + 1)));
			}
			else {
				bool flag = true;
				for (uint32_t i = 0; i < left.size(); ++i) {
					if (name[i] != left[i]) flag = false;
				}
			}
		}
		std::string NewName;
		if (maxIndex == -1) {
			NewName = left + "(0)";
		}
		else
		{
			++maxIndex;
			NewName = left + std::to_string(maxIndex) + right;
		}
		return NewName;
	}

	Entity Scene::DuplicateEntity(Entity entity)
	{
		//Copy Name
		std::string name = entity.GetName();
		name = CreateNewName(name, this);

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
		CopyComponentIfExists<MaterialTreeComponent>(newEntity, entity);
		CopyComponentIfExists<RigidBody3DComponent>(newEntity, entity);

		return newEntity;
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

	void Scene::SetSkyBox(Ref<CubeMap> skyBox)
	{
		m_skyBox = skyBox;
	}

	void Scene::MarkMeshEntityIDDirty()
	{
		m_Registry.view<StaticMeshComponent>().each(
			[=](auto entity, auto& smc)
		{
			smc.mesh.SetEntityDirty(true);
		});
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

	template<>
	void Scene::OnComponentAdded<MaterialTreeComponent>(Entity entity, MaterialTreeComponent& component)
	{
		//from path to load the logic material tree
		std::string path = component.path;

		//read the material logic node, and compile the shader

	}

	template<>
	void Scene::OnComponentAdded<RigidBody3DComponent>(Entity entity, RigidBody3DComponent& component)
	{

	}
}

