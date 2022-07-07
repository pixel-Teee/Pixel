#pragma once

#include "SceneCamera.h"
#include "Pixel/Core/UUID.h"
#include "Pixel/Renderer/Texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "Pixel/Renderer/3D/Model.h"
#include "Pixel/Renderer/3D/Material.h"

namespace Pixel {
	
	struct IDComponent
	{
		UUID ID;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
	};

	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
		:Tag(tag){}
	};

	struct TransformComponent
	{
		glm::vec3 Translation = {0.0f, 0.0f, 0.0f};
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation) : Translation(translation){}

		glm::mat4 GetTransform() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));
			
			return glm::translate(glm::mat4(1.0f), Translation)
			* rotation
			* glm::scale(glm::mat4(1.0f), Scale);
		}

		void SetScale(glm::vec3 scale)
		{
			Scale = scale;
		}
	};

	struct SpriteRendererComponent
	{
		glm::vec4 Color{1.0f, 1.0f, 1.0f, 1.0f};
		Ref<Texture2D> Texture;
		float TilingFactor = 1.0f;
		//Ref<Material>
		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& color) : Color(color) {}

		operator glm::vec4& () { return Color; }
		operator const glm::vec4& () const { return Color; };
	};

	struct CameraComponent
	{
		SceneCamera camera;
		//think about move to scene
		bool Primary = true;
		//when OnResize, whether is SetViewport
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
		//CameraComponent(const glm::mat4& projection);
	};

	//Forward declartion
	class ScriptableEntity;
	struct NativeScriptComponent
	{
		//------
		ScriptableEntity* Instance = nullptr;

		//ScriptableEntity*(*InstantiateScript)();
		//void (*DestroyScript)(NativeScriptComponent*);

		std::string m_path;

		void Instantiate(std::string path);

		void Destroy();

		
		/*template<typename T>
		void Bind()
		{
			InstantiateScript = [](){ return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
		}*/
		//------
		
		/*lua_State* m_pLuaState;

		void (*InstantiateScript)(NativeScriptComponent* nsc);
		void (*DestroyScript)(NativeScriptComponent*);

		void Bind()
		{
			InstantiateScript = [](NativeScriptComponent* nsc) { nsc->m_pLuaState = luaL_newstate();
				luaL_openlibs(nsc->m_pLuaState);
			};
			DestroyScript = [](NativeScriptComponent* nsc) { lua_close(nsc->m_pLuaState); };
		}*/
	};

	//Physics

	struct Rigidbody2DComponent
	{
		enum class BodyType {
			Static = 0,
			Dynamic,
			Kinematic
		};

		BodyType Type = BodyType::Static;

		bool FixedRotation = false;

		//Storage for runtime
		void* RuntimeBody = nullptr;

		Rigidbody2DComponent() = default;
		Rigidbody2DComponent(const Rigidbody2DComponent&) = default;
	};

	struct BoxCollider2DComponent
	{
		glm::vec2 Offset = {0.0f, 0.0f};
		glm::vec2 Size = {0.5f, 0.5f};

		//TODO: move into the physics material in the future maybe
		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;

		//Storage for runtime
		void* RuntimeFixture = nullptr;

		BoxCollider2DComponent() = default;
		BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
	};

	struct MaterialComponent
	{
		Ref<Texture2D> Albedo;
		Ref<Texture2D> Roughness;
		Ref<Texture2D> NormalMap;
		Ref<Texture2D> Metallic;
		Ref<Texture2D> Emissive;

		std::string albedoPath;
		std::string roughnessPath;
		std::string normalMapPath;
		std::string metallicPath;
		std::string emissivePath;

		//constant
		glm::vec3 gAlbedo = { 1.0f, 1.0f, 1.0f };
		glm::vec3 gNormal = { 1.0f, 1.0f, 1.0f };
		float gRoughness = 1.0f;
		float gMetallic = 1.0f;
		float gEmissive = 1.0f;
		bool HaveNormal = false;

		MaterialComponent()
		{
			uint32_t whiteTextureData = 0xffffff;
			uint32_t whiteTextureData2 = 0xff;
			Albedo = Texture2D::Create(4, 1, 1, ImageFormat::PX_FORMAT_R8G8B8A8_UNORM, (const void*)&whiteTextureData);
			NormalMap = Texture2D::Create(4, 1, 1, ImageFormat::PX_FORMAT_R8G8B8A8_UNORM, (const void*)&whiteTextureData);
			Roughness = Texture2D::Create(1, 1, 1, ImageFormat::PX_FORMAT_R8_UNORM, (const void*)&whiteTextureData2);
			Metallic = Texture2D::Create(1, 1, 1, ImageFormat::PX_FORMAT_R8_UNORM, (const void*)&whiteTextureData2);
			Emissive = Texture2D::Create(1, 1, 1, ImageFormat::PX_FORMAT_R8_UNORM, (const void*)&whiteTextureData2);
		}
		MaterialComponent(const MaterialComponent&) = default;

		MaterialComponent(std::string& AlbedoPath, std::string& NormalMapPath,
			std::string& RoughnessPath, std::string& MetallicPath, std::string& EmissivePath,
			glm::vec3 albedo, glm::vec3 normal, float roughness, float metallic, float emissive, bool haveNormal)
		{
			uint32_t whiteTextureData = 0xffffffff;
			uint32_t whiteTextureData2 = 0xff;
			albedoPath = AlbedoPath;
			normalMapPath = NormalMapPath;
			roughnessPath = RoughnessPath;
			metallicPath = MetallicPath;
			emissivePath = EmissivePath;

			gAlbedo = albedo;
			gNormal = normal;
			gRoughness = roughness;
			gMetallic = metallic;
			gEmissive = emissive;
			HaveNormal = haveNormal;
			
			if(AlbedoPath != "")
				Albedo = Texture2D::Create(AlbedoPath);
			else
			{
				Albedo = Texture2D::Create(4, 1, 1, ImageFormat::PX_FORMAT_R8G8B8A8_UNORM, (const void*)&whiteTextureData);
			}
			if(NormalMapPath != "")
				NormalMap = Texture2D::Create(NormalMapPath);
			else
			{
				NormalMap = Texture2D::Create(4, 1, 1, ImageFormat::PX_FORMAT_R8G8B8A8_UNORM, (const void*)&whiteTextureData);
			}
			if(RoughnessPath != "")
				Roughness = Texture2D::Create(RoughnessPath);
			else
			{
				Roughness = Texture2D::Create(1, 1, 1, ImageFormat::PX_FORMAT_R8_UNORM, (const void*)&whiteTextureData2);
			}
			if(MetallicPath != "")
				Metallic = Texture2D::Create(MetallicPath);
			else
			{
				Metallic = Texture2D::Create(1, 1, 1, ImageFormat::PX_FORMAT_R8_UNORM, (const void*)&whiteTextureData2);
			}
			if(EmissivePath != "")
				Emissive = Texture2D::Create(EmissivePath);
			else
			{
				Emissive = Texture2D::Create(1, 1, 1, ImageFormat::PX_FORMAT_R8_UNORM, (const void*)&whiteTextureData2);
			}
		}
	};

	//---experimental component---
	struct MaterialTreeComponent {
		std::string path;//material tree component file path
		Ref<Material> m_pMaterial;
		Ref<MaterialInstance> m_pMaterialInstance;
		MaterialTreeComponent(const std::string& Path) {
			path = Path;
			//from this path to load the material file's logic node
			//there add one shader main function
			m_pMaterial = CreateRef<Material>("Material", Material::MUT_GEO);
			m_pMaterialInstance = CreateRef<MaterialInstance>(m_pMaterial);
		}
	};
	//---experimental component---

	struct StaticMeshComponent
	{
		Model mesh;	
		//std::string path;

		//char path[256];
		std::string path;

		//Editor's Attribute
		int currentItem;

		StaticMeshComponent() = default;
		StaticMeshComponent(const StaticMeshComponent&) = default;
		StaticMeshComponent(const std::string& Path)
		{
			//memcpy(path, Path.c_str(), sizeof(char) * Path.size());
			//path[Path.size()] = '\0';
			path = Path;
		}
	};

	enum class LightType
	{
		PointLight = 0,
		DirectLight = 1,
		SpotLight = 2
	};

	//TODO£ºtemporary there only have a point light
	struct LightComponent
	{
		LightType lightType;
		//LightColor
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		bool GenerateShadowMap = false;//temporarily for direct light

		//------for point light's volume radius------
		float constant = 1.0f;
		float linear = 0.09f;
		float quadratic = 0.032f;
		//------for point light's volume radius------

		LightComponent() = default;
		LightComponent(const LightComponent&) = default;

		float GetSphereLightVolumeRadius()
		{
			float lightMax = glm::compMax(color);

			//light volume sphere radius
			float radius = 
			(-linear + std::sqrtf(linear * linear - 4 * quadratic * (constant - (256.0f / 5.0f) * lightMax))) / (2 * quadratic);

			return radius;
		}
	};

	struct RigidBody3DComponent
	{
		enum ShapeType {
			BoxShape,
			SphereShape,
			ConvexHull
		};
		ShapeType m_shapeType;
		//fix:complete type
		void* m_body = nullptr;
		void* m_shape = nullptr;

		float m_bodyMass;
		glm::vec3 m_bodyInertia;
		float m_restitution = 1.0f;
		float m_friction = 0.0f;

		float m_Radius;
		float m_HalfLength;

		RigidBody3DComponent() = default;
		RigidBody3DComponent(const RigidBody3DComponent& rhs)
		{
			//copy pointer???
			//need to fix this function
			m_bodyMass = rhs.m_bodyMass;
			m_bodyInertia = rhs.m_bodyInertia;
			m_restitution = rhs.m_restitution;
			m_friction = rhs.m_friction;

			m_Radius = rhs.m_Radius;
			m_HalfLength = rhs.m_HalfLength;
		}
	};
}
