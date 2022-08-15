#pragma once

#include <codecvt>
#include <locale>

#include "Pixel/Core/Singleton.h"

namespace Pixel {
	class Texture2D;
	class Model;
	class SubMaterial;
	class AssetManager : public Singleton<AssetManager>
	{
	public:
		enum class AssetType
		{
			TEXTURE,
			MODEL,
			SCENE,
			MATERIAL
		};

		AssetManager();

		virtual ~AssetManager();

		void LoadRegistry();//load asset registry

		void SaveRegistry();//save asset registry

		void AddTextureToAssetRegistry(const std::wstring& filePath);

		void AddSceneToAssetRegistry(const std::string& filePath);

		void AddMaterialToAssetRegistry(const std::wstring& filePath);

		std::string GetAssetRegistryPath(const std::string& physicalFilePath);

		std::string GetAssetPhysicalPath(const std::string& filePath);

		bool IsInAssetRegistry(std::string filepath);

		bool IsInMaterialAssetRegistry(std::string virtualPath);

		Ref<Texture2D> GetTexture(const std::string& assetRegistry);

		Ref<Model> GetModel(const std::string& modelRegistry);

		Ref<SubMaterial> GetMaterial(const std::string& materialRegistry);

		void AddModelToAssetRegistry(const std::string& filePath);

		void CreateSubMaterial(const std::string& physicalPath);

		std::string  to_string(std::wstring wstr);

		std::wstring to_wsrting(std::string str);
	private:
		std::map<std::string, std::string> m_textureAssetRegistry;//asset registry path <=> asset physical path

		std::map<std::string, std::string> m_AssetRegistryTexture;//asset physical path <=> asset registry path

		//texture
		std::map<std::string, Ref<Texture2D>> m_textures;//asset registry path <=> asset texture

		//model
		std::map<std::string, Ref<Model>> m_models;//asset registry path <=> asset model

		std::map<std::string, std::string> m_AssetRegistryModel;//asset physical path <=> asset registry path

		std::map<std::string, std::string> m_ModelAssetRegistry;//asset registry path <=> asset physical path

		//scene
		std::map<std::string, std::string> m_AssetRegistryScene;//asset physical path <=> asset registry path

		std::map<std::string, std::string> m_SceneAssetRegistry;//asset registry path <=> asset physical path


		std::map<std::string, Ref<SubMaterial>> m_Materials;

		std::map<std::string, std::string> m_AssetRegistryToPhysicalMaterial;
		
		std::map<std::string, std::string> m_PhysicalToAssetRegistryMaterial;

		//------converter------
		using convert_t = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_t, wchar_t> m_strconverter;
		//------converter------
	};
}