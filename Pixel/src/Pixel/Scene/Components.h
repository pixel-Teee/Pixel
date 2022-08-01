#pragma once

//------other library------
#include "entt.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
//------other library------

//------my library------
#include "Pixel/Core/UUID.h"
#include "Pixel/Scene/SceneCamera.h"
#include "Pixel/Renderer/Texture.h"
#include "Pixel/Renderer/3D/Model.h"
#include "Pixel/Renderer/3D/Material.h"

#include "Pixel/Scene/Components/IDComponent.h"
#include "Pixel/Scene/Components/TagComponent.h"
#include "Pixel/Scene/Components/TransformComponent.h"
#include "Pixel/Scene/Components/CameraComponent.h"
#include "Pixel/Scene/Components/SpriteRendererComponent.h"
#include "Pixel/Scene/Components/NativeScriptComponent.h"
#include "Pixel/Scene/Components/Rigidbody2DComponent.h"
//------my library------

namespace Pixel {

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

	enum class ShadingModel
	{
		SM_Unlit = 0,
		SM_DefaultLit = 1,
		SM_ClearCoat = 2,
		SM_NRP = 3,
		SM_MAX = 4
	};

	struct MaterialComponent
	{
		ShadingModel shadingModel = ShadingModel::SM_DefaultLit;
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
		float gEmissive = 0.2f;
		bool HaveNormal = false;
		float ClearCoat = 1.0f;
		float ClearCoatRoughness = 1.0f;

		bool nextFrameNeedLoadTexture[5];

		MaterialComponent()
		{
			uint32_t whiteTextureData = 0xffffff;
			uint32_t whiteTextureData2 = 0xff;
			Albedo = Texture2D::Create(4, 1, 1, ImageFormat::PX_FORMAT_R8G8B8A8_UNORM, (const void*)&whiteTextureData);
			NormalMap = Texture2D::Create(4, 1, 1, ImageFormat::PX_FORMAT_R8G8B8A8_UNORM, (const void*)&whiteTextureData);
			Roughness = Texture2D::Create(1, 1, 1, ImageFormat::PX_FORMAT_R8_UNORM, (const void*)&whiteTextureData2);
			Metallic = Texture2D::Create(1, 1, 1, ImageFormat::PX_FORMAT_R8_UNORM, (const void*)&whiteTextureData2);
			Emissive = Texture2D::Create(1, 1, 1, ImageFormat::PX_FORMAT_R8_UNORM, (const void*)&whiteTextureData2);

			for (uint32_t i = 0; i < 5; ++i)
				nextFrameNeedLoadTexture[i] = false;
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

			for (uint32_t i = 0; i < 5; ++i)
				nextFrameNeedLoadTexture[i] = false;
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
		bool DisplayLightVolume = false;//temporarily for point light
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

		glm::mat4 GetTransformComponent(TransformComponent transformComponent)
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(transformComponent.Rotation));

			return glm::translate(glm::mat4(1.0f), transformComponent.Translation)
				* rotation
				* glm::scale(glm::mat4(1.0f), transformComponent.Scale * GetSphereLightVolumeRadius());
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
