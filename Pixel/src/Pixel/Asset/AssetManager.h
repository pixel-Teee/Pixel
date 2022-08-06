#pragma once

#include <codecvt>
#include <locale>

#include "Pixel/Core/Singleton.h"

namespace Pixel {
	class Texture2D;

	class AssetManager : public Singleton<AssetManager>
	{
	public:
		AssetManager();

		virtual ~AssetManager();

		void LoadRegistry();//load asset registry

		void SaveRegistry();//save asset registry

		void AddTextureToAssetRegistry(const std::wstring& filePath);

		std::string GetAssetRegistryPath(const std::string& physicalFilePath);

		bool IsInAssetRegistry(std::string filepath);

		Ref<Texture2D> GetTexture(const std::string& assetRegistry);
	private:
		std::map<std::string, std::string> m_textureAssetRegistry;//asset registry path <=> asset physical path

		std::map<std::string, std::string> m_AssetRegistryTexture;//asset physical path <=> asset registry path

		//texture
		std::map<std::string, Ref<Texture2D>> m_textures;//asset registry path <=> asset texture

		//model


		//------converter------
		using convert_t = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_t, wchar_t> m_strconverter;

		std::string  to_string(std::wstring wstr);
		std::wstring to_wsrting(std::string str);
		//------converter------
	};
}