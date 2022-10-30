#pragma once

#include <codecvt>
#include <locale>

#include "Pixel/Core/Singleton.h"
#include "Pixel/Renderer/ShaderMap.h"

namespace Pixel {
	class Texture2D;
	class Model;
	struct SubMaterial;
	class Material;
	class MaterialInstance;
	//class ShaderMap::ShaderSet;
	class AssetManager : public Singleton<AssetManager>
	{
	public:

		AssetManager();

		virtual ~AssetManager();

		void LoadRegistry();//load asset registry

		void SaveRegistry();//save asset registry

		void DeleteRegistry(const std::string& virtualPath);

		void AddTextureToAssetRegistry(const std::wstring& filePath);

		void AddSceneToAssetRegistry(const std::string& filePath);

		void AddMaterialToAssetRegistry(const std::wstring& physicalPath);

		void AddTestMaterialToAssetRegistry(const std::wstring& physicalPath);

		void AddMaterialInstanceToAssetRegistry(const std::wstring& physicalPath);

		std::string GetVirtualPath(const std::string& physicalFilePath);

		std::string GetAssetPhysicalPath(const std::string& filePath);

		bool IsInAssetRegistry(std::string physicalPath);

		bool IsInMaterialAssetRegistry(std::string virtualPath);

		bool IsInTestMaterialAssetRegistry(std::string& virtualPath);

		bool IsInMaterialInstanceAssetRegistry(std::string& virtualPath);

		Ref<Texture2D> GetTexture(const std::string& virtualPath);

		Ref<Model> GetModel(const std::string& modelRegistry);

		Ref<SubMaterial> GetMaterial(const std::string& virtualPath);

		Ref<Material> GetTestMaterial(const std::string& virtualPath);

		Ref<MaterialInstance> GetMaterialInstance(const std::string& virtualPath);

		//in terms of shader name (material name) to get shader set
		Ref<ShaderSet> GetVertexShaderSet(const std::string& shaderName);

		Ref<ShaderSet> GetPixelShaderSet(const std::string& shaderName);

		std::map<std::string, Ref<Material>>& GetMaterials();

		std::map<std::string, Ref<MaterialInstance>>& GetMaterialInstances();

		std::map<std::string, Ref<Texture2D>>& GetTextures();

		std::map<std::string, Ref<Texture2D>>& GetMaterialPreviewImages();

		std::map<std::string, std::string>& GetMaterialVirtualPathToPreviewImagePhysicalPath();

		void AddModelToAssetRegistry(const std::string& physicalPath);

		void CreateSubMaterial(const std::string& physicalPath, Ref<SubMaterial> pSubMaterial);

		void CreateTestMaterial(const std::string& physicalPath, Ref<Material> pMaterial);

		void CreateMaterialInstance(const std::string& physicalPath, Ref<MaterialInstance> pMaterialInstance);

		void UpdateMaterial(const std::string& physicalPath, Ref<SubMaterial> pSubMaterial);//difference update

		std::string to_string(std::wstring wstr);

		std::wstring to_wsrting(std::string str);

		std::map<std::string, std::string>& GetTextureAssetRegistry();

		std::map<std::string, std::string>& GetMaterialAssetRegistry();

		std::map<std::string, std::string>& GetTestMaterialAssetRegistry();

		std::map<std::string, std::string>& GetMaterialInstanceAssetRegistry();
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

		//test material
		std::map<std::string, std::string> m_VirtualPathToPhysicalPathTestMaterial;

		std::map<std::string, std::string> m_PhysicalPathToVirtualPathTestMaterial;

		std::map<std::string, Ref<Material>> m_TestMaterials;

		//for preview image(materials)
		std::map<std::string, Ref<Texture2D>> m_PreviewImages;

		std::map<std::string, std::string> m_MaterialVirtualPathToPreviewImagePhysicalPath;

		//material instance
		std::map<std::string, std::string> m_VirtualPathToPhysicalPathMaterialInstance;

		std::map<std::string, std::string> m_PhysicalPathToVirtualPathMaterialInstance;

		std::map<std::string, Ref<MaterialInstance>> m_MaterialInstances;//asset virtual path <=> material instance

		//------converter------
		using convert_t = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_t, wchar_t> m_strconverter;
		//------converter------

		//------shader map------
		ShaderMap m_VertexShaderMap;//just to save entire shader map
		ShaderMap m_PixelShaderMap;
		//------shader map------
	};
}