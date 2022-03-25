#pragma once

#include "SceneCamera.h"
#include "Pixel/Core/UUID.h"
#include "Pixel/Renderer/Texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "SceneCamera.h"
#include "Pixel/Renderer/3D/Model.h"

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
		ScriptableEntity* Instance = nullptr;

		ScriptableEntity*(*InstantiateScript)();
		void (*DestroyScript)(NativeScriptComponent*);

		template<typename T>
		void Bind()
		{
			InstantiateScript = [](){ return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
		}
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

		MaterialComponent()
		{
			uint32_t whiteTextureData = 0xffffff;
			uint32_t whiteTextureData2 = 0xff;
			Albedo = Texture2D::Create(1, 1, TextureFormat::RGB);
			Albedo->SetData(&whiteTextureData, 3);
			NormalMap = Texture2D::Create(1, 1, TextureFormat::RGB);
			NormalMap->SetData(&whiteTextureData, 3);
			Roughness = Texture2D::Create(1, 1, TextureFormat::RED);
			Roughness->SetData(&whiteTextureData2, 1);
			Metallic = Texture2D::Create(1, 1, TextureFormat::RED);
			Metallic->SetData(&whiteTextureData2, 1);
			Emissive = Texture2D::Create(1, 1, TextureFormat::RED);
			Emissive->SetData(&whiteTextureData2, 1);
		}
		MaterialComponent(const MaterialComponent&) = default;

		MaterialComponent(std::string& albedoPath, std::string& normalMapPath,
			std::string& roughnessPath, std::string& metallicPath, std::string& emissivePath)
		{
			uint32_t whiteTextureData = 0xffffff;
			uint32_t whiteTextureData2 = 0xff;
			if(albedoPath != "")
				Albedo = Texture2D::Create(albedoPath);
			else
			{
				Albedo = Texture2D::Create(1, 1, TextureFormat::RGB);
				Albedo->SetData(&whiteTextureData, 3);
			}
			if(normalMapPath != "")
				NormalMap = Texture2D::Create(normalMapPath);
			else
			{
				NormalMap = Texture2D::Create(1, 1, TextureFormat::RGB);
				NormalMap->SetData(&whiteTextureData, 3);
			}
			if(roughnessPath != "")
				Roughness = Texture2D::Create(roughnessPath);
			else
			{
				Roughness = Texture2D::Create(1, 1, TextureFormat::RED);
				Roughness->SetData(&whiteTextureData2, 1);
			}
			if(metallicPath != "")
				Metallic = Texture2D::Create(metallicPath);
			else
			{
				Metallic = Texture2D::Create(1, 1, TextureFormat::RED);
				Metallic->SetData(&whiteTextureData2, 1);
			}
			if(emissivePath != "")
				Emissive = Texture2D::Create(emissivePath);
			else
			{
				Emissive = Texture2D::Create(1, 1, TextureFormat::RED);
				Emissive->SetData(&whiteTextureData2, 1);
			}
		}
	};

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

	//TODO£ºtemporary there only have a point light
	struct LightComponent
	{
		//LightColor
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		float constant = 1.0f;
		float linear = 0.09f;
		float quadratic = 0.032f;

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
}
