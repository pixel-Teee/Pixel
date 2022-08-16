#include "pxpch.h"

#include "AssetManager.h"

#include <filesystem>

//------other library------
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
//------other library------

//------my library------
#include "Pixel/Renderer/Texture.h"
#include "Pixel/Renderer/3D/Model.h"
#include "Pixel/Scene/Components/MaterialComponent.h"
//------my library------

namespace Pixel {
	extern const std::filesystem::path g_AssetPath;

	AssetManager::AssetManager()
	{
		LoadRegistry();
	}

	AssetManager::~AssetManager()
	{
		m_textureAssetRegistry.clear();
		m_AssetRegistryTexture.clear();
		m_SceneAssetRegistry.clear();
		m_textureAssetRegistry.clear();
		m_ModelAssetRegistry.clear();
		m_AssetRegistryModel.clear();
		m_textures.clear();
	}

	std::string AssetManager::GetAssetRegistryPath(const std::string& physicalFilePath)
	{
		if(m_AssetRegistryTexture.find(physicalFilePath) != m_AssetRegistryTexture.end())
			return m_AssetRegistryTexture[physicalFilePath];
		if (m_AssetRegistryModel.find(physicalFilePath) != m_AssetRegistryModel.end())
			return m_AssetRegistryModel[physicalFilePath];
		if(m_AssetRegistryScene.find(physicalFilePath) != m_AssetRegistryScene.end())
			return m_SceneAssetRegistry[physicalFilePath];
		if (m_PhysicalToAssetRegistryMaterial.find(physicalFilePath) != m_PhysicalToAssetRegistryMaterial.end())
			return m_PhysicalToAssetRegistryMaterial[physicalFilePath];
		return "";
	}

	std::string AssetManager::GetAssetPhysicalPath(const std::string& filePath)
	{
		if (m_textureAssetRegistry.find(filePath) != m_textureAssetRegistry.end())
			return m_textureAssetRegistry[filePath];
		if (m_AssetRegistryModel.find(filePath) != m_AssetRegistryModel.end())
			return m_AssetRegistryModel[filePath];
		if (m_AssetRegistryScene.find(filePath) != m_AssetRegistryScene.end())
			return m_SceneAssetRegistry[filePath];
		if (m_AssetRegistryToPhysicalMaterial.find(filePath) != m_AssetRegistryToPhysicalMaterial.end())
			return m_AssetRegistryToPhysicalMaterial[filePath];
		return "";
	}

	void AssetManager::LoadRegistry()
	{
		rapidjson::Document doc;

		std::ifstream stream("assets/AssetRegistry/AssetRegistry.json");
		std::stringstream strStream;
		strStream << stream.rdbuf();

		if (!doc.Parse(strStream.str().data()).HasParseError())
		{
			if (doc.HasMember("Texture") && doc["Texture"].IsArray())
			{
				rapidjson::Value& array = doc["Texture"].GetArray();

				std::string registyPath = "Texture";

				for (auto iter = array.Begin();
					iter != array.End(); ++iter)
				{
					const rapidjson::Value& attribute = *iter;
					PX_CORE_ASSERT(attribute.IsObject(), "attribute is not object!");

					for (rapidjson::Value::ConstMemberIterator itr2 = attribute.MemberBegin();
						itr2 != attribute.MemberEnd(); ++itr2)
					{
						std::string assetRegistryPath = itr2->name.GetString();//don't include the asset prefix, etc. asset\\scene\\fish.pixel
						m_textureAssetRegistry[assetRegistryPath] = itr2->value.GetString();
						m_AssetRegistryTexture[itr2->value.GetString()] = assetRegistryPath;
					}			
				}
			}

			if (doc.HasMember("Scene") && doc["Scene"].IsArray())
			{
				rapidjson::Value& array = doc["Scene"].GetArray();

				std::string registyPath = "Scene";

				for (auto iter = array.Begin();
					iter != array.End(); ++iter)
				{
					const rapidjson::Value& attribute = *iter;
					PX_CORE_ASSERT(attribute.IsObject(), "attribute is not object!");

					for (rapidjson::Value::ConstMemberIterator itr2 = attribute.MemberBegin();
						itr2 != attribute.MemberEnd(); ++itr2)
					{
						std::string assetRegistryPath = itr2->name.GetString();//don't include the asset prefix, etc. asset\\scene\\fish.pixel
						m_SceneAssetRegistry[assetRegistryPath] = itr2->value.GetString();
						m_AssetRegistryScene[itr2->value.GetString()] = assetRegistryPath;
					}
				}			
			}

			if (doc.HasMember("Model") && doc["Model"].IsArray())
			{
				rapidjson::Value& array = doc["Model"].GetArray();

				std::string registyPath = "Model";

				for (auto iter = array.Begin();
					iter != array.End(); ++iter)
				{
					const rapidjson::Value& attribute = *iter;
					PX_CORE_ASSERT(attribute.IsObject(), "attribute is not object!");

					for (rapidjson::Value::ConstMemberIterator itr2 = attribute.MemberBegin();
						itr2 != attribute.MemberEnd(); ++itr2)
					{
						std::string assetRegistryPath = itr2->name.GetString();//don't include the asset prefix, etc. asset\\scene\\fish.pixel
						m_ModelAssetRegistry[assetRegistryPath] = itr2->value.GetString();
						m_AssetRegistryModel[itr2->value.GetString()] = assetRegistryPath;
					}
				}
			}

			if(doc.HasMember("Material") && doc["Material"].IsArray())
			{
				rapidjson::Value& array = doc["Material"].GetArray();

				std::string registyPath = "Material";

				for (auto iter = array.Begin();
					iter != array.End(); ++iter)
				{
					const rapidjson::Value& attribute = *iter;
					PX_CORE_ASSERT(attribute.IsObject(), "attribute is not object!");

					for (rapidjson::Value::ConstMemberIterator itr2 = attribute.MemberBegin();
						itr2 != attribute.MemberEnd(); ++itr2)
					{
						std::string assetRegistryPath = itr2->name.GetString();//don't include the asset prefix, etc. asset\\scene\\fish.pixel
						m_AssetRegistryToPhysicalMaterial[assetRegistryPath] = itr2->value.GetString();
						m_PhysicalToAssetRegistryMaterial[itr2->value.GetString()] = assetRegistryPath;
					}
				}
			}
		}

		stream.close();
	}

	void AssetManager::SaveRegistry()
	{
		rapidjson::StringBuffer strBuf;
		rapidjson::Writer<rapidjson::StringBuffer> writer(strBuf);

		writer.StartObject();
		writer.Key("Texture");
		writer.StartArray();
		for (auto& item : m_textureAssetRegistry)
		{
			writer.StartObject();
			writer.Key(item.first.c_str());
			writer.String(item.second.c_str());
			writer.EndObject();
		}
		writer.EndArray();

		writer.Key("Scene");
		writer.StartArray();
		for (auto& item : m_SceneAssetRegistry)
		{
			writer.StartObject();
			writer.Key(item.first.c_str());
			writer.String(item.second.c_str());
			writer.EndObject();
		}
		writer.EndArray();

		writer.Key("Model");
		writer.StartArray();
		for (auto& item : m_ModelAssetRegistry)
		{
			writer.StartObject();
			writer.Key(item.first.c_str());
			writer.String(item.second.c_str());
			writer.EndObject();
		}
		writer.EndArray();

		writer.Key("Material");
		writer.StartArray();
		for (auto& item : m_AssetRegistryToPhysicalMaterial)
		{
			writer.StartObject();
			writer.Key(item.first.c_str());
			writer.String(item.second.c_str());
			writer.EndObject();
		}
		writer.EndArray();
		writer.EndObject();

		std::string data = strBuf.GetString();
		std::ofstream fout("assets/AssetRegistry/AssetRegistry.json");
		fout << data.c_str();
		fout.close();
	}

	void AssetManager::AddTextureToAssetRegistry(const std::wstring& filePath)
	{
		//file path is texture's physical file path

		//extract the filename of filePath as the asset registry
		std::string convertedFilePath = to_string(filePath);

		size_t pos = convertedFilePath.find_last_of("\\");

		if (pos != std::string::npos)
		{
			//extract the filename
			std::string fileName;
			size_t dotPos = convertedFilePath.substr(pos).find_last_of(".");
			if (dotPos != std::string::npos)
			{
				fileName = convertedFilePath.substr(pos + 1).substr(0, dotPos - 1);
			}
			else
			{
				fileName = convertedFilePath.substr(pos);
			}

			std::filesystem::path physicalPath(filePath);

			auto relativePath = std::filesystem::relative(physicalPath, g_AssetPath);

			std::string relativePathString = relativePath.string();

			//construct the virtual path
			std::string virtualFilePath = fileName;
			m_textureAssetRegistry.insert({ virtualFilePath, relativePathString });
			m_AssetRegistryTexture.insert({ relativePathString, virtualFilePath });

			SaveRegistry();
		}
	}

	void AssetManager::AddSceneToAssetRegistry(const std::string& filePath)
	{
		//file path is scene's physical file path

		//extract the filename of filePath as the asset registry
		
		size_t pos = filePath.find_last_of("\\");

		if (pos != std::string::npos)
		{
			//extract the filename
			std::string fileName;
			size_t dotPos = filePath.substr(pos).find_last_of(".");
			if (dotPos != std::string::npos)
			{
				fileName = filePath.substr(pos + 1).substr(0, dotPos - 1);
			}
			else
			{
				fileName = filePath.substr(pos);
			}

			std::filesystem::path physicalPath(filePath);

			auto relativePath = std::filesystem::relative(physicalPath, g_AssetPath);

			std::string relativePathString = relativePath.string();

			//construct the virtual path
			std::string virtualFilePath = fileName;
			m_SceneAssetRegistry.insert({ virtualFilePath, relativePathString });
			m_AssetRegistryScene.insert({ relativePathString, virtualFilePath });

			SaveRegistry();
		}
	}

	void AssetManager::AddMaterialToAssetRegistry(const std::wstring& filePath)
	{
		//file path is texture's physical file path

		//extract the filename of filePath as the asset registry
		std::string convertedFilePath = to_string(filePath);

		size_t pos = convertedFilePath.find_last_of("\\");

		if (pos != std::string::npos)
		{
			//extract the filename
			std::string fileName;
			size_t dotPos = convertedFilePath.substr(pos).find_last_of(".");
			if (dotPos != std::string::npos)
			{
				fileName = convertedFilePath.substr(pos + 1).substr(0, dotPos - 1);
			}
			else
			{
				fileName = convertedFilePath.substr(pos);
			}

			std::filesystem::path physicalPath(filePath);

			auto relativePath = std::filesystem::relative(physicalPath, g_AssetPath);

			std::string relativePathString = relativePath.string();

			//construct the virtual path
			std::string virtualFilePath = fileName;
			m_AssetRegistryToPhysicalMaterial.insert({ virtualFilePath, relativePathString });
			m_PhysicalToAssetRegistryMaterial.insert({ relativePathString, virtualFilePath });

			SaveRegistry();
		}
	}

	bool AssetManager::IsInAssetRegistry(std::string filepath)
	{
		if (m_AssetRegistryTexture.find(filepath) != m_AssetRegistryTexture.end())
		{
			return true;
		}
		if (m_AssetRegistryModel.find(filepath) != m_AssetRegistryModel.end())
		{
			return true;
		}
		if (m_AssetRegistryTexture.find(filepath) != m_AssetRegistryTexture.end())
		{
			return true;
		}
		if(m_PhysicalToAssetRegistryMaterial.find(filepath) != m_PhysicalToAssetRegistryMaterial.end())
		{
			return true;
		}
		return false;
	}

	bool AssetManager::IsInMaterialAssetRegistry(std::string virtualPath)
	{
		if (m_AssetRegistryToPhysicalMaterial.find(virtualPath) != m_AssetRegistryToPhysicalMaterial.end())
			return true;
		return false;
	}

	Ref<Texture2D> AssetManager::GetTexture(const std::string& assetRegistry)
	{
		//first to query the m_textures
		if (m_textures.find(assetRegistry) != m_textures.end())
		{
			return m_textures[assetRegistry];
		}
		else
		{
			std::string physicalAssetPath;
			//from the m_textureAssetRegistry get the physical path to load texture
			if (m_textureAssetRegistry.find(assetRegistry) != m_textureAssetRegistry.end())
			{
				physicalAssetPath = m_textureAssetRegistry[assetRegistry];

				//load texture to map
				m_textures[assetRegistry] = Texture2D::Create(g_AssetPath.string() + "\\" + physicalAssetPath);
			}
			else
			{
				//TODO:open the small window to tell user to fix asset registry
			}
			return m_textures[assetRegistry];
		}
	}

	Ref<Model> AssetManager::GetModel(const std::string& modelRegistry)
	{
		//first to query the m_Models
		if (m_models.find(modelRegistry) != m_models.end())
		{
			return m_models[modelRegistry];
		}
		else
		{
			std::string physicalAssetPath;
			//from the m_textureAssetRegistry get the physical path to load texture
			if (m_ModelAssetRegistry.find(modelRegistry) != m_ModelAssetRegistry.end())
			{
				physicalAssetPath = m_ModelAssetRegistry[modelRegistry];

				//load texture to map
				m_models[modelRegistry] = Model::Create(g_AssetPath.string() + "\\" + physicalAssetPath);
			}
			else
			{
				//TODO:open the small window to tell user to fix asset registry
			}
			return m_models[modelRegistry];
		}
	}

	Ref<SubMaterial> AssetManager::GetMaterial(const std::string& materialRegistry)
	{
		if (m_Materials.find(materialRegistry) != m_Materials.end())
		{
			return m_Materials[materialRegistry];
		}
		else
		{
			std::string physicalAssetPath;

			if (m_AssetRegistryToPhysicalMaterial.find(materialRegistry) != m_AssetRegistryToPhysicalMaterial.end())
			{
				physicalAssetPath = m_AssetRegistryToPhysicalMaterial[materialRegistry];

				//load sub material
				m_Materials[materialRegistry] = CreateRef<SubMaterial>();

				Reflect::TypeDescriptor* typeDesc = Reflect::TypeResolver<SubMaterial>::get();

				rapidjson::Document doc;

				//from the physical asset path to load the material and post load
				std::ifstream stream(g_AssetPath.string() + "\\" + physicalAssetPath);
				std::stringstream strStream;
				strStream << stream.rdbuf();
				if (!doc.Parse(strStream.str().data()).HasParseError())
				{
					//read the sub material
					if (doc.HasMember(typeDesc->name) && doc[typeDesc->name].IsObject())
					{
						typeDesc->Read(doc[typeDesc->name], m_Materials[materialRegistry].get(), nullptr);
					}
				}
				stream.close();

				//post load the sub material
				m_Materials[materialRegistry]->PostLoad();
			}
			else
			{
				//TODO:open the small window to tell user to fix asset registry
			}
			return m_Materials[materialRegistry];
		}
	}

	void AssetManager::AddModelToAssetRegistry(const std::string& filePath)
	{
		size_t pos = filePath.find_last_of("\\");

		if (pos != std::string::npos)
		{
			//extract the filename
			std::string fileName;
			size_t dotPos = filePath.substr(pos).find_last_of(".");
			if (dotPos != std::string::npos)
			{
				fileName = filePath.substr(pos + 1).substr(0, dotPos - 1);
			}
			else
			{
				fileName = filePath.substr(pos);
			}

			std::filesystem::path physicalPath(filePath);

			auto relativePath = std::filesystem::relative(physicalPath, g_AssetPath);

			std::string relativePathString = relativePath.string();

			//construct the virtual path
			std::string virtualFilePath = fileName;
			m_ModelAssetRegistry.insert({ virtualFilePath, relativePathString });
			m_AssetRegistryModel.insert({ relativePathString, virtualFilePath });

			SaveRegistry();
		}
	}

	void AssetManager::CreateSubMaterial(const std::string& physicalPath, Ref<SubMaterial> pSubMaterial)
	{
		//create a temporary default sub material and write to file

		rapidjson::StringBuffer strBuf;
		rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(strBuf);

		writer.StartObject();
		Reflect::TypeDescriptor* typeDesc = Reflect::TypeResolver<SubMaterial>().get();
		writer.Key(typeDesc->name);
		writer.StartObject();
		typeDesc->Write(writer, pSubMaterial.get(), nullptr);//write a new sub material file
		writer.EndObject();
		writer.EndObject();
		std::string data = strBuf.GetString();
		std::ofstream fout(physicalPath);
		fout << data.c_str();
		fout.close();
	}

	void AssetManager::UpdateMaterial(const std::string& physicalPath, Ref<SubMaterial> pSubMaterial)
	{
		//first, to find the needed update material
		std::string assetRegistryPath = GetAssetRegistryPath(physicalPath);

		if (m_Materials.find(assetRegistryPath) != m_Materials.end())
		{
			Ref<SubMaterial> pNeededUpdateSubMaterial = m_Materials[assetRegistryPath];

			//TODO:in the future, will in be terms of the difference, automatically update the material

			if (pNeededUpdateSubMaterial->albedoMapPath != pSubMaterial->albedoMapPath)
			{
				pNeededUpdateSubMaterial->albedoMap = pSubMaterial->albedoMap;
				pNeededUpdateSubMaterial->albedoMapPath = pSubMaterial->albedoMapPath;
			}

			if (pNeededUpdateSubMaterial->normalMapPath != pSubMaterial->normalMapPath)
			{
				pNeededUpdateSubMaterial->normalMap = pSubMaterial->normalMap;
				pNeededUpdateSubMaterial->normalMapPath = pSubMaterial->normalMapPath;
			}

			if (pNeededUpdateSubMaterial->metallicMapPath != pSubMaterial->metallicMapPath)
			{
				pNeededUpdateSubMaterial->metallicMap = pSubMaterial->metallicMap;
				pNeededUpdateSubMaterial->metallicMapPath = pSubMaterial->metallicMapPath;
			}

			if(pNeededUpdateSubMaterial->roughnessMapPath != pSubMaterial->roughnessMapPath)
			{
				pNeededUpdateSubMaterial->roughnessMap = pSubMaterial->roughnessMap;
				pNeededUpdateSubMaterial->roughnessMapPath = pSubMaterial->roughnessMapPath;
			}

			if(pNeededUpdateSubMaterial->aoMapPath != pSubMaterial->aoMapPath)
			{
				pNeededUpdateSubMaterial->aoMap = pSubMaterial->aoMap;
				pNeededUpdateSubMaterial->aoMapPath = pSubMaterial->aoMapPath;
			}

			if (pNeededUpdateSubMaterial->gAlbedo != pSubMaterial->gAlbedo)
				pNeededUpdateSubMaterial->gAlbedo = pSubMaterial->gAlbedo;
			if (pNeededUpdateSubMaterial->gNormal != pSubMaterial->gNormal)
				pNeededUpdateSubMaterial->gNormal = pSubMaterial->gNormal;
			if (pNeededUpdateSubMaterial->HaveNormal != pSubMaterial->HaveNormal)
				pNeededUpdateSubMaterial->HaveNormal = pSubMaterial->HaveNormal;
			if (pNeededUpdateSubMaterial->gMetallic != pSubMaterial->gMetallic)
				pNeededUpdateSubMaterial->gMetallic = pSubMaterial->gMetallic;
			if (pNeededUpdateSubMaterial->gRoughness != pSubMaterial->gRoughness)
				pNeededUpdateSubMaterial->gRoughness = pSubMaterial->gRoughness;
			if (pNeededUpdateSubMaterial->gAo != pSubMaterial->gAo)
				pNeededUpdateSubMaterial->gAo = pSubMaterial->gAo;
			if (pNeededUpdateSubMaterial->ClearCoat != pSubMaterial->ClearCoat)
				pNeededUpdateSubMaterial->ClearCoat = pSubMaterial->ClearCoat;
			if (pNeededUpdateSubMaterial->ClearCoatRoughness != pSubMaterial->ClearCoat)
				pNeededUpdateSubMaterial->ClearCoatRoughness = pSubMaterial->ClearCoatRoughness;
			if (pNeededUpdateSubMaterial->IsTransparent != pSubMaterial->IsTransparent)
				pNeededUpdateSubMaterial->IsTransparent = pSubMaterial->IsTransparent;
		}
	}

	std::string AssetManager::to_string(std::wstring wstr)
	{
		return m_strconverter.to_bytes(wstr);
	}

	std::wstring AssetManager::to_wsrting(std::string str)
	{
		return m_strconverter.from_bytes(str);
	}

	std::map<std::string, std::string>& AssetManager::GetMaterialAssetRegistry()
	{
		return m_AssetRegistryToPhysicalMaterial;
	}
}
