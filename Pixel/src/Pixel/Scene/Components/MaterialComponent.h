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

	class DescriptorHeap;
	class DescriptorHandle;
	//------material component is consist of material------
	struct SubMaterial {
		ShadingModel shadingModel = ShadingModel::SM_DefaultLit;
		Ref<Texture2D> albedoMap;
		Ref<Texture2D> normalMap;
		Ref<Texture2D> metallicMap;
		Ref<Texture2D> roughnessMap;
		Ref<Texture2D> aoMap;

		std::string albedoMapPath;
		std::string normalMapPath;
		std::string metallicMapPath;
		std::string roughnessMapPath;
		std::string aoMapPath;

		//constant
		glm::vec3 gAlbedo = { 1.0f, 1.0f, 1.0f };
		glm::vec3 gNormal = { 1.0f, 1.0f, 1.0f };
		bool HaveNormal = false;
		float gMetallic = 1.0f;
		float gRoughness = 1.0f;
		float gAo = 0.0f;
		float ClearCoat = 1.0f;
		float ClearCoatRoughness = 1.0f;

		bool nextFrameNeedLoadTexture[5];//load in the next frame

		bool IsTransparent = false;//will draw in the transparent pass

				//texture descriptor heap
		Ref<DescriptorHeap> m_pDescriptorHeap;
		Ref<DescriptorHandle> m_pTextureFirstHandle;

		SubMaterial();

		//------just for editor camera use------
		SubMaterial(const std::string& AlbedoMapPath, const std::string& NormalMapPath, const std::string& MetallicMapPath,
		const std::string& RoughnessMapPath, const std::string& AoMapPath, bool haveNormal);
		//------just for editor camera use------

		void PostLoad();

		RTTR_ENABLE()
		RTTR_REGISTRATION_FRIEND
	};
	//------material component is consist of material------
}