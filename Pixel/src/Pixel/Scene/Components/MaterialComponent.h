#pragma once

//------my library------
#include "Pixel/Renderer/Texture.h"
#include "Pixel/Asset/AssetManager.h"
//------my library------

namespace Pixel {
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

			if (AlbedoPath != "")
				Albedo = Texture2D::Create(AlbedoPath);
			else
			{
				Albedo = Texture2D::Create(4, 1, 1, ImageFormat::PX_FORMAT_R8G8B8A8_UNORM, (const void*)&whiteTextureData);
			}
			if (NormalMapPath != "")
				NormalMap = Texture2D::Create(NormalMapPath);
			else
			{
				NormalMap = Texture2D::Create(4, 1, 1, ImageFormat::PX_FORMAT_R8G8B8A8_UNORM, (const void*)&whiteTextureData);
			}
			if (RoughnessPath != "")
				Roughness = Texture2D::Create(RoughnessPath);
			else
			{
				Roughness = Texture2D::Create(1, 1, 1, ImageFormat::PX_FORMAT_R8_UNORM, (const void*)&whiteTextureData2);
			}
			if (MetallicPath != "")
				Metallic = Texture2D::Create(MetallicPath);
			else
			{
				Metallic = Texture2D::Create(1, 1, 1, ImageFormat::PX_FORMAT_R8_UNORM, (const void*)&whiteTextureData2);
			}
			if (EmissivePath != "")
				Emissive = Texture2D::Create(EmissivePath);
			else
			{
				Emissive = Texture2D::Create(1, 1, 1, ImageFormat::PX_FORMAT_R8_UNORM, (const void*)&whiteTextureData2);
			}

			for (uint32_t i = 0; i < 5; ++i)
				nextFrameNeedLoadTexture[i] = false;
		}

		void PostLoad()
		{
			uint32_t whiteTextureData = 0xffffffff;
			uint32_t whiteTextureData2 = 0xff;

			if (albedoPath != "")
				Albedo = AssetManager::GetSingleton().GetTexture(albedoPath);
			else
			{
				Albedo = Texture2D::Create(4, 1, 1, ImageFormat::PX_FORMAT_R8G8B8A8_UNORM, (const void*)&whiteTextureData);
			}
			if (normalMapPath != "")
				NormalMap = AssetManager::GetSingleton().GetTexture(normalMapPath);
			else
			{
				NormalMap = Texture2D::Create(4, 1, 1, ImageFormat::PX_FORMAT_R8G8B8A8_UNORM, (const void*)&whiteTextureData);
			}
			if (roughnessPath != "")
				Roughness = AssetManager::GetSingleton().GetTexture(roughnessPath);
			else
			{
				Roughness = Texture2D::Create(1, 1, 1, ImageFormat::PX_FORMAT_R8_UNORM, (const void*)&whiteTextureData2);
			}
			if (metallicPath != "")
				Metallic = AssetManager::GetSingleton().GetTexture(metallicPath);
			else
			{
				Metallic = Texture2D::Create(1, 1, 1, ImageFormat::PX_FORMAT_R8_UNORM, (const void*)&whiteTextureData2);
			}
			if (emissivePath != "")
				Emissive = AssetManager::GetSingleton().GetTexture(emissivePath);
			else
			{
				Emissive = Texture2D::Create(1, 1, 1, ImageFormat::PX_FORMAT_R8_UNORM, (const void*)&whiteTextureData2);
			}

			for (uint32_t i = 0; i < 5; ++i)
				nextFrameNeedLoadTexture[i] = false;
		}

		REFLECT()
	};
}