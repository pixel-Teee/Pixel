#include "pxpch.h"

#include "AssetManager.h"

#include <filesystem>

//------other library------
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>
//------other library------

//------my library------
#include "Pixel/Renderer/Texture.h"
#include "Pixel/Renderer/3D/Model.h"
#include "Pixel/Renderer/3D/Material/Material.h"
#include "Pixel/Scene/Components/MaterialComponent.h"
#include "Pixel/Scene/SceneSerializer.h"
#include "Pixel/Renderer/3D/Material/MaterialInstance.h"
//------my library------

namespace Pixel {
	extern const std::filesystem::path g_AssetPath;

	AssetManager::AssetManager()
	{
		LoadRegistry();
	}

	AssetManager::~AssetManager()
	{
		m_VirtualPathToPhysicalPathTexture.clear();
		m_PhysicalPathToVirtualPathTexture.clear();
		m_PhysicalPathToVirtualPathScene.clear();
		m_VirtualPathToPhysicalPathTexture.clear();
		m_PhysicalPathToVirtualPathModel.clear();
		m_VirtualPathToPhysicalPathModel.clear();
		m_PhysicalPathToVirtualPathMaterial.clear();
		m_VirtualPathToPhysicalPathMaterial.clear();
		m_textures.clear();
	}

	std::string AssetManager::GetVirtualPath(const std::string& physicalFilePath)
	{
		if(m_PhysicalPathToVirtualPathTexture.find(physicalFilePath) != m_PhysicalPathToVirtualPathTexture.end())
			return m_PhysicalPathToVirtualPathTexture[physicalFilePath];
		if (m_PhysicalPathToVirtualPathModel.find(physicalFilePath) != m_PhysicalPathToVirtualPathModel.end())
			return m_PhysicalPathToVirtualPathModel[physicalFilePath];
		if(m_PhysicalPathToVirtualPathScene.find(physicalFilePath) != m_PhysicalPathToVirtualPathScene.end())
			return m_PhysicalPathToVirtualPathScene[physicalFilePath];
		if (m_PhysicalPathToVirtualPathMaterial.find(physicalFilePath) != m_PhysicalPathToVirtualPathMaterial.end())
			return m_PhysicalPathToVirtualPathMaterial[physicalFilePath];
		if (m_PhysicalPathToVirtualPathTestMaterial.find(physicalFilePath) != m_PhysicalPathToVirtualPathTestMaterial.end())
			return m_PhysicalPathToVirtualPathTestMaterial[physicalFilePath];
		return "";
	}

	std::string AssetManager::GetAssetPhysicalPath(const std::string& filePath)
	{
		if (m_VirtualPathToPhysicalPathTexture.find(filePath) != m_VirtualPathToPhysicalPathTexture.end())
			return m_VirtualPathToPhysicalPathTexture[filePath];
		if (m_VirtualPathToPhysicalPathModel.find(filePath) != m_VirtualPathToPhysicalPathModel.end())
			return m_VirtualPathToPhysicalPathModel[filePath];
		if (m_VirtualPathToPhysicalPathScene.find(filePath) != m_VirtualPathToPhysicalPathScene.end())
			return m_PhysicalPathToVirtualPathScene[filePath];
		if (m_VirtualPathToPhysicalPathMaterial.find(filePath) != m_VirtualPathToPhysicalPathMaterial.end())
			return m_VirtualPathToPhysicalPathMaterial[filePath];
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
						m_VirtualPathToPhysicalPathTexture[assetRegistryPath] = itr2->value.GetString();
						m_PhysicalPathToVirtualPathTexture[itr2->value.GetString()] = assetRegistryPath;
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
						m_VirtualPathToPhysicalPathScene[assetRegistryPath] = itr2->value.GetString();
						m_PhysicalPathToVirtualPathScene[itr2->value.GetString()] = assetRegistryPath;
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
						m_VirtualPathToPhysicalPathModel[assetRegistryPath] = itr2->value.GetString();
						m_PhysicalPathToVirtualPathModel[itr2->value.GetString()] = assetRegistryPath;
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
						m_VirtualPathToPhysicalPathMaterial[assetRegistryPath] = itr2->value.GetString();
						m_PhysicalPathToVirtualPathMaterial[itr2->value.GetString()] = assetRegistryPath;
					}
				}
			}

			if (doc.HasMember("TestMaterial") && doc["TestMaterial"].IsArray())
			{
				rapidjson::Value& array = doc["TestMaterial"].GetArray();

				std::string registyPath = "TestMaterial";

				for (auto iter = array.Begin();
					iter != array.End(); ++iter)
				{
					const rapidjson::Value& attribute = *iter;
					PX_CORE_ASSERT(attribute.IsObject(), "attribute is not object!");

					for (rapidjson::Value::ConstMemberIterator itr2 = attribute.MemberBegin();
						itr2 != attribute.MemberEnd(); ++itr2)
					{
						std::string assetRegistryPath = itr2->name.GetString();//don't include the asset prefix, etc. asset\\scene\\fish.pixel
						m_VirtualPathToPhysicalPathTestMaterial[assetRegistryPath] = itr2->value.GetString();
						m_PhysicalPathToVirtualPathTestMaterial[itr2->value.GetString()] = assetRegistryPath;
					}
				}
			}

			if (doc.HasMember("MaterialInstance") && doc["MaterialInstance"].IsArray())
			{
				rapidjson::Value& array = doc["MaterialInstance"].GetArray();

				std::string registyPath = "MaterialInstance";

				for (auto iter = array.Begin();
					iter != array.End(); ++iter)
				{
					const rapidjson::Value& attribute = *iter;
					PX_CORE_ASSERT(attribute.IsObject(), "attribute is not object!");

					for (rapidjson::Value::ConstMemberIterator itr2 = attribute.MemberBegin();
						itr2 != attribute.MemberEnd(); ++itr2)
					{
						std::string assetRegistryPath = itr2->name.GetString();//don't include the asset prefix, etc. asset\\scene\\fish.pixel
						m_VirtualPathToPhysicalPathMaterialInstance[assetRegistryPath] = itr2->value.GetString();
						m_PhysicalPathToVirtualPathMaterialInstance[itr2->value.GetString()] = assetRegistryPath;
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
		for (auto& item : m_VirtualPathToPhysicalPathTexture)
		{
			writer.StartObject();
			writer.Key(item.first.c_str());
			writer.String(item.second.c_str());
			writer.EndObject();
		}
		writer.EndArray();

		writer.Key("Scene");
		writer.StartArray();
		for (auto& item : m_VirtualPathToPhysicalPathScene)
		{
			writer.StartObject();
			writer.Key(item.first.c_str());
			writer.String(item.second.c_str());
			writer.EndObject();
		}
		writer.EndArray();

		writer.Key("Model");
		writer.StartArray();
		for (auto& item : m_VirtualPathToPhysicalPathModel)
		{
			writer.StartObject();
			writer.Key(item.first.c_str());
			writer.String(item.second.c_str());
			writer.EndObject();
		}
		writer.EndArray();

		writer.Key("Material");
		writer.StartArray();
		for (auto& item : m_VirtualPathToPhysicalPathMaterial)
		{
			writer.StartObject();
			writer.Key(item.first.c_str());
			writer.String(item.second.c_str());
			writer.EndObject();
		}
		writer.EndArray();

		writer.Key("TestMaterial");
		writer.StartArray();
		for (auto& item : m_VirtualPathToPhysicalPathTestMaterial)
		{
			writer.StartObject();
			writer.Key(item.first.c_str());
			writer.String(item.second.c_str());
			writer.EndObject();
		}
		writer.EndArray();

		writer.Key("MaterialInstance");
		writer.StartArray();
		for (auto& item : m_VirtualPathToPhysicalPathMaterialInstance)
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
			std::string virtualFilePath = "Texture\\" + fileName;
			m_VirtualPathToPhysicalPathTexture.insert({ virtualFilePath, relativePathString });
			m_PhysicalPathToVirtualPathTexture.insert({ relativePathString, virtualFilePath });

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
			std::string virtualFilePath = "Scene\\" + fileName;
			m_PhysicalPathToVirtualPathScene.insert({ virtualFilePath, relativePathString });
			m_VirtualPathToPhysicalPathScene.insert({ relativePathString, virtualFilePath });

			SaveRegistry();
		}
	}

	void AssetManager::AddMaterialToAssetRegistry(const std::wstring& physicalPath)
	{
		//file path is texture's physical file path

		//extract the filename of filePath as the asset registry
		std::string convertedFilePath = to_string(physicalPath);

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

			std::filesystem::path PhysicalPath(physicalPath);

			auto relativePath = std::filesystem::relative(PhysicalPath, g_AssetPath);

			std::string relativePathString = relativePath.string();

			//construct the virtual path
			std::string virtualFilePath = "Material\\" + fileName;
			m_VirtualPathToPhysicalPathMaterial.insert({ virtualFilePath, relativePathString });
			m_PhysicalPathToVirtualPathMaterial.insert({ relativePathString, virtualFilePath });

			SaveRegistry();
		}
	}

	void AssetManager::AddTestMaterialToAssetRegistry(const std::wstring& physicalPath)
	{
		//file path is texture's physical file path

		//extract the filename of filePath as the asset registry
		std::string convertedFilePath = to_string(physicalPath);

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

			std::filesystem::path PhysicalPath(physicalPath);

			auto relativePath = std::filesystem::relative(PhysicalPath, g_AssetPath);

			std::string relativePathString = relativePath.string();

			//construct the virtual path
			std::string virtualFilePath = "TestMaterial\\" + fileName;
			m_VirtualPathToPhysicalPathTestMaterial.insert({ virtualFilePath, relativePathString });
			m_PhysicalPathToVirtualPathTestMaterial.insert({ relativePathString, virtualFilePath });

			SaveRegistry();
		}
	}

	void AssetManager::AddMaterialInstanceToAssetRegistry(const std::wstring& physicalPath)
	{
		std::string convertedFilePath = to_string(physicalPath);

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

			std::filesystem::path PhysicalPath(physicalPath);

			auto relativePath = std::filesystem::relative(PhysicalPath, g_AssetPath);

			std::string relativePathString = relativePath.string();

			//construct the virtual path
			std::string virtualFilePath = "MaterialInstance\\" + fileName;
			m_VirtualPathToPhysicalPathMaterialInstance.insert({ virtualFilePath, relativePathString });
			m_PhysicalPathToVirtualPathMaterialInstance.insert({ relativePathString, virtualFilePath });

			SaveRegistry();
		}
	}

	bool AssetManager::IsInAssetRegistry(std::string filepath)
	{
		if (m_PhysicalPathToVirtualPathTexture.find(filepath) != m_PhysicalPathToVirtualPathTexture.end())
		{
			return true;
		}
		if (m_PhysicalPathToVirtualPathModel.find(filepath) != m_PhysicalPathToVirtualPathModel.end())
		{
			return true;
		}
		if (m_PhysicalPathToVirtualPathTexture.find(filepath) != m_PhysicalPathToVirtualPathTexture.end())
		{
			return true;
		}
		if(m_PhysicalPathToVirtualPathMaterial.find(filepath) != m_PhysicalPathToVirtualPathMaterial.end())
		{
			return true;
		}
		if (m_PhysicalPathToVirtualPathTestMaterial.find(filepath) != m_PhysicalPathToVirtualPathTestMaterial.end())
		{
			return true;
		}
		if (m_PhysicalPathToVirtualPathMaterialInstance.find(filepath) != m_PhysicalPathToVirtualPathMaterialInstance.end())
		{
			return true;
		}
		return false;
	}

	bool AssetManager::IsInMaterialAssetRegistry(std::string virtualPath)
	{
		if (m_VirtualPathToPhysicalPathMaterial.find(virtualPath) != m_VirtualPathToPhysicalPathMaterial.end())
			return true;
		return false;
	}

	Ref<Pixel::Texture2D> AssetManager::GetTexture(const std::string& virtualPath)
	{
		//first to query the m_textures
		if (m_textures.find(virtualPath) != m_textures.end())
		{
			return m_textures[virtualPath];
		}
		else
		{
			std::string physicalPath;
			//from the m_textureAssetRegistry get the physical path to load texture
			if (m_VirtualPathToPhysicalPathTexture.find(virtualPath) != m_VirtualPathToPhysicalPathTexture.end())
			{
				physicalPath = m_VirtualPathToPhysicalPathTexture[virtualPath];

				//load texture to map
				m_textures[virtualPath] = Texture2D::Create(g_AssetPath.string() + "\\" + physicalPath);
			}
			else
			{
				//TODO:open the small window to tell user to fix asset registry
			}
			return m_textures[virtualPath];
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
			if (m_VirtualPathToPhysicalPathModel.find(modelRegistry) != m_VirtualPathToPhysicalPathModel.end())
			{
				physicalAssetPath = m_VirtualPathToPhysicalPathModel[modelRegistry];

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

	Ref<Pixel::SubMaterial> AssetManager::GetMaterial(const std::string& virtualPath)
	{
		if (m_Materials.find(virtualPath) != m_Materials.end())
		{
			return m_Materials[virtualPath];
		}
		else
		{
			std::string physicalPath;

			if (m_VirtualPathToPhysicalPathMaterial.find(virtualPath) != m_VirtualPathToPhysicalPathMaterial.end())
			{
				physicalPath = m_VirtualPathToPhysicalPathMaterial[virtualPath];

				//load sub material
				m_Materials[virtualPath] = CreateRef<SubMaterial>();

				rapidjson::Document doc;

				rttr::type subMaterialType = rttr::type::get<SubMaterial>();

				//from the physical asset path to load the material and post load
				std::ifstream stream(g_AssetPath.string() + "\\" + physicalPath);
				std::stringstream strStream;
				strStream << stream.rdbuf();
				if (!doc.Parse(strStream.str().data()).HasParseError())
				{
					//read the sub material
					if (doc.HasMember(subMaterialType.get_name().to_string().c_str()) && doc[subMaterialType.get_name().to_string().c_str()].IsObject())
					{
						SceneSerializer::FromJsonRecursive(*m_Materials[virtualPath], doc[subMaterialType.get_name().to_string().c_str()], false);
					}
				}
				stream.close();

				//post load the sub material
				//m_Materials[virtualPath]->PostLoad();
			}
			else
			{
				//TODO:open the small window to tell user to fix asset registry
			}
			return m_Materials[virtualPath];
		}
	}

	Ref<Material> AssetManager::GetTestMaterial(const std::string& virtualPath)
	{
		if (m_TestMaterials.find(virtualPath) != m_TestMaterials.end())
		{
			return m_TestMaterials[virtualPath];
		}
		else
		{
			std::string physicalPath;

			if (m_VirtualPathToPhysicalPathTestMaterial.find(virtualPath) != m_VirtualPathToPhysicalPathTestMaterial.end())
			{
				physicalPath = m_VirtualPathToPhysicalPathTestMaterial[virtualPath];

				//load test material
				m_TestMaterials[virtualPath] = CreateRef<Material>();

				//Reflect::TypeDescriptor* typeDesc = Reflect::TypeResolver<Material>::get();
				//
				//rapidjson::Document doc;
				//
				////from the physical asset path to load the material and post load
				//std::ifstream stream(g_AssetPath.string() + "\\" + physicalPath);
				//std::stringstream strStream;
				//strStream << stream.rdbuf();
				//if (!doc.Parse(strStream.str().data()).HasParseError())
				//{
				//	//read the sub material
				//	if (doc.HasMember(typeDesc->name) && doc[typeDesc->name].IsObject())
				//	{
				//		typeDesc->Read(doc[typeDesc->name], m_TestMaterials[virtualPath].get(), nullptr, false);
				//	}
				//}
				//stream.close();

				rapidjson::Document doc;

				std::ifstream stream(g_AssetPath.string() + "\\" + physicalPath);
				std::stringstream strStream;
				strStream << stream.rdbuf();

				rttr::type materialType = rttr::type::get<Material>();

				if (!doc.Parse(strStream.str().data()).HasParseError())
				{
					//TODO:need to fix, need a perfect reflection scheme
					if (doc.HasMember(materialType.get_name().to_string().c_str()) && doc[materialType.get_name().to_string().c_str()].IsObject())
					{
						SceneSerializer::FromJsonRecursive(*m_TestMaterials[virtualPath], doc[materialType.get_name().to_string().c_str()], true);
					}
				}
				stream.close();

				for (size_t i = 0; i < m_TestMaterials[virtualPath]->GetShaderFunction().size(); ++i)
				{
					//if(m_pMaterial->GetShaderFunction(i))

					if (m_TestMaterials[virtualPath]->GetShaderFunction()[i]->GetFunctioNodeId() == 1)
					{
						m_TestMaterials[virtualPath]->m_pShaderMainFunction = std::reinterpret_pointer_cast<ShaderMainFunction>(m_TestMaterials[virtualPath]->GetShaderFunction()[i]);
					}
				}

				m_TestMaterials[virtualPath]->PostLink();
			}
			else
			{
				//TODO:open the small window to tell user to fix asset registry
			}
			return m_TestMaterials[virtualPath];
		}
	}

	Ref<MaterialInstance> AssetManager::GetMaterialInstance(const std::string& virtualPath)
	{
		if (m_MaterialInstances.find(virtualPath) != m_MaterialInstances.end())
		{
			return m_MaterialInstances[virtualPath];
		}
		else
		{
			std::string physicalPath;

			if (m_VirtualPathToPhysicalPathMaterialInstance.find(virtualPath) != m_VirtualPathToPhysicalPathMaterialInstance.end())
			{
				physicalPath = m_VirtualPathToPhysicalPathMaterialInstance[virtualPath];

				//load test material
				m_MaterialInstances[virtualPath] = CreateRef<MaterialInstance>();

				//Reflect::TypeDescriptor* typeDesc = Reflect::TypeResolver<Material>::get();
				//
				//rapidjson::Document doc;
				//
				////from the physical asset path to load the material and post load
				//std::ifstream stream(g_AssetPath.string() + "\\" + physicalPath);
				//std::stringstream strStream;
				//strStream << stream.rdbuf();
				//if (!doc.Parse(strStream.str().data()).HasParseError())
				//{
				//	//read the sub material
				//	if (doc.HasMember(typeDesc->name) && doc[typeDesc->name].IsObject())
				//	{
				//		typeDesc->Read(doc[typeDesc->name], m_TestMaterials[virtualPath].get(), nullptr, false);
				//	}
				//}
				//stream.close();

				//post load the sub material
			}
			else
			{
				//TODO:open the small window to tell user to fix asset registry
			}
			return m_MaterialInstances[virtualPath];
		}
	}

	void AssetManager::AddModelToAssetRegistry(const std::string& physicalPath)
	{
		size_t pos = physicalPath.find_last_of("\\");

		if (pos != std::string::npos)
		{
			//extract the filename
			std::string fileName;
			size_t dotPos = physicalPath.substr(pos).find_last_of(".");
			if (dotPos != std::string::npos)
			{
				fileName = physicalPath.substr(pos + 1).substr(0, dotPos - 1);
			}
			else
			{
				fileName = physicalPath.substr(pos);
			}

			std::filesystem::path PhysicalPath(physicalPath);

			auto relativePath = std::filesystem::relative(PhysicalPath, g_AssetPath);

			std::string relativePathString = relativePath.string();

			//construct the virtual path
			std::string virtualFilePath = "Model\\" + fileName;
			m_VirtualPathToPhysicalPathModel.insert({ virtualFilePath, relativePathString });
			m_PhysicalPathToVirtualPathModel.insert({ relativePathString, virtualFilePath });

			SaveRegistry();
		}
	}

	void AssetManager::CreateSubMaterial(const std::string& physicalPath, Ref<SubMaterial> pSubMaterial)
	{
		//create a temporary default sub material and write to file

		//rapidjson::StringBuffer strBuf;
		//rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(strBuf);
		//
		//writer.StartObject();
		//Reflect::TypeDescriptor* typeDesc = Reflect::TypeResolver<SubMaterial>().get();
		//typeDesc->Write(writer, pSubMaterial.get(), typeDesc->name, false);//write a new sub material file
		//writer.EndObject();
		//std::string data = strBuf.GetString();
		//std::ofstream fout(physicalPath);
		//fout << data.c_str();
		//fout.close();
		rapidjson::StringBuffer strBuf;
		rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(strBuf);
		writer.StartObject();
		rttr::type subMaterialType = rttr::type::get<SubMaterial>();
		writer.Key(subMaterialType.get_name().to_string().c_str());

		//write sub material to file
		SceneSerializer::ToJsonRecursive(*pSubMaterial, writer);

		writer.EndObject();
		std::string data = strBuf.GetString();
		std::ofstream fout(physicalPath);
		fout << data.c_str();
		fout.close();
	}

	void AssetManager::CreateTestMaterial(const std::string& physicalPath, Ref<Material> pMaterial)
	{
		//create a temporary default test material and write to file

		rapidjson::StringBuffer strBuf;
		rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(strBuf);
		writer.StartObject();
		rttr::type materialType = rttr::type::get<Material>();
		writer.Key(materialType.get_name().to_string().c_str());
		SceneSerializer::ToJsonRecursive(*pMaterial, writer, true);
		writer.EndObject();
		std::string data = strBuf.GetString();
		std::ofstream fout(physicalPath);
		fout << data.c_str();
		fout.close();
	}

	void AssetManager::CreateMaterialInstance(const std::string& physicalPath, Ref<MaterialInstance> pMaterialInstance)
	{
		rapidjson::StringBuffer strBuf;
		rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(strBuf);
		writer.StartObject();
		rttr::type materialType = rttr::type::get<MaterialInstance>();
		writer.Key(materialType.get_name().to_string().c_str());
		SceneSerializer::ToJsonRecursive(*pMaterialInstance, writer, true);
		writer.EndObject();
		std::string data = strBuf.GetString();
		std::ofstream fout(physicalPath);
		fout << data.c_str();
		fout.close();
	}

	void AssetManager::UpdateMaterial(const std::string& physicalPath, Ref<SubMaterial> pSubMaterial)
	{
		//first, to find the needed update material
		std::string assetRegistryPath = GetVirtualPath(physicalPath);

		if (m_Materials.find(assetRegistryPath) != m_Materials.end())
		{
			Ref<SubMaterial> pNeededUpdateSubMaterial = m_Materials[assetRegistryPath];

			//TODO:in the future, will in be terms of the difference, automatically update the material

			if(pNeededUpdateSubMaterial->shadingModel != pSubMaterial->shadingModel)
			{
				pNeededUpdateSubMaterial->shadingModel = pSubMaterial->shadingModel;
			}

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
			if (pNeededUpdateSubMaterial->ClearCoatRoughness != pSubMaterial->ClearCoatRoughness)
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
		return m_VirtualPathToPhysicalPathMaterial;
	}
	std::map<std::string, std::string>& AssetManager::GetTestMaterialAssetRegistry()
	{
		return m_VirtualPathToPhysicalPathTestMaterial;
	}

	std::map<std::string, std::string>& AssetManager::GetMaterialInstanceAssetRegistry()
	{
		return m_VirtualPathToPhysicalPathMaterialInstance;
	}

	bool AssetManager::IsInTestMaterialAssetRegistry(std::string& virtualPath)
	{
		if (m_VirtualPathToPhysicalPathTestMaterial.find(virtualPath) != m_VirtualPathToPhysicalPathTestMaterial.end())
			return true;
		return false;
	}

	bool AssetManager::IsInMaterialInstanceAssetRegistry(std::string& virtualPath)
	{
		if (m_VirtualPathToPhysicalPathMaterialInstance.find(virtualPath) != m_VirtualPathToPhysicalPathMaterialInstance.end())
			return true;
		return false;
	}

}
