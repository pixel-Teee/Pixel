#pragma once

#include <codecvt>
#include <locale>

#include "Pixel/Core/Singleton.h"

namespace Pixel {
	class Texture2D;
	class Model;
	struct SubMaterial;
	class AssetManager : public Singleton<AssetManager>
	{
	public:

		AssetManager();

		virtual ~AssetManager();

		void LoadRegistry();//load asset registry

		void SaveRegistry();//save asset registry

		void AddTextureToAssetRegistry(const std::wstring& filePath);

		void AddSceneToAssetRegistry(const std::string& filePath);

		void AddMaterialToAssetRegistry(const std::wstring& physicalPath);

		std::string GetVirtualPath(const std::string& physicalFilePath);

		std::string GetAssetPhysicalPath(const std::string& filePath);

		bool IsInAssetRegistry(std::string filepath);

		bool IsInMaterialAssetRegistry(std::string virtualPath);

		Ref<Texture2D> GetTexture(const std::string& virtualPath);

		Ref<Model> GetModel(const std::string& modelRegistry);

		Ref<SubMaterial> GetMaterial(const std::string& virtualPath);

		void AddModelToAssetRegistry(const std::string& physicalPath);

		void CreateSubMaterial(const std::string& physicalPath, Ref<SubMaterial> pSubMaterial);

		void UpdateMaterial(const std::string& physicalPath, Ref<SubMaterial> pSubMaterial);//difference update

		std::string  to_string(std::wstring wstr);

		std::wstring to_wsrting(std::string str);

		std::map<std::string, std::string>& GetMaterialAssetRegistry();
	private:
		std::map<std::string, std::string> m_VirtualPathToPhysicalPathTexture;//asset virtual path <=> asset physical path

		std::map<std::string, std::string> m_PhysicalPathToVirtualPathTexture;//asset physical path <=> asset virtual path

		//texture
		std::map<std::string, Ref<Texture2D>> m_textures;//asset virtual path <=> asset texture

		//model
		std::map<std::string, Ref<Model>> m_models;//asset virtual path <=> asset model

		std::map<std::string, std::string> m_VirtualPathToPhysicalPathModel;//asset physical path <=> asset virtual path

		std::map<std::string, std::string> m_PhysicalPathToVirtualPathModel;//asset virtual path <=> asset physical path

		//scene
		std::map<std::string, std::string> m_VirtualPathToPhysicalPathScene;//asset physical path <=> asset virtual path

		std::map<std::string, std::string> m_PhysicalPathToVirtualPathScene;//asset virtual path <=> asset physical path


		std::map<std::string, Ref<SubMaterial>> m_Materials;

		std::map<std::string, std::string> m_VirtualPathToPhysicalPathMaterial;
		
		std::map<std::string, std::string> m_PhysicalPathToVirtualPathMaterial;

		//------converter------
		using convert_t = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_t, wchar_t> m_strconverter;
		//------converter------
	};
}