#include "pxpch.h"

#include "MaterialComponent.h"

namespace Pixel
{
	
	SubMaterial::SubMaterial()
	{
		uint32_t DefaultAlbedoMapTextureData[4] = { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff };

		uint32_t DefaultNormalMapTextureData[4] = { 0xffffffff, 0, 0xffffffff, 0 };

		uint8_t DefaultMetallicMapTextureData, DefaultRoughnessMapTextureData, DefaultAoMapTextureData = 0xff;

		//------create texture------
		albedoMap = Texture2D::Create(8, 2, 2, ImageFormat::PX_FORMAT_R8G8B8A8_UNORM, DefaultAlbedoMapTextureData);
		normalMap = Texture2D::Create(8, 2, 2, ImageFormat::PX_FORMAT_R8G8B8A8_UNORM, DefaultNormalMapTextureData);
		metallicMap = Texture2D::Create(1, 1, 1, ImageFormat::PX_FORMAT_R8_UNORM, &DefaultMetallicMapTextureData);
		roughnessMap = Texture2D::Create(1, 1, 1, ImageFormat::PX_FORMAT_R8_UNORM, &DefaultRoughnessMapTextureData);
		aoMap = Texture2D::Create(1, 1, 1, ImageFormat::PX_FORMAT_R8_UNORM, &DefaultAoMapTextureData);
		//------create texture------

		for (uint32_t i = 0; i < 5; ++i)
			nextFrameNeedLoadTexture[i] = false;
	}

	REFLECT_STRUCT_BEGIN(SubMaterial)
	REFLECT_STRUCT_MEMBER(shadingModel)
	//REFLECT_STRUCT_MEMBER(albedoMap)
	//REFLECT_STRUCT_MEMBER(normalMap)
	//REFLECT_STRUCT_MEMBER(metallicMap)
	//REFLECT_STRUCT_MEMBER(roughnessMap)
	//REFLECT_STRUCT_MEMBER(aoMap)
	REFLECT_STRUCT_MEMBER(albedoMapPath)
	REFLECT_STRUCT_MEMBER(normalMapPath)
	REFLECT_STRUCT_MEMBER(metallicMapPath)
	REFLECT_STRUCT_MEMBER(roughnessMapPath)
	REFLECT_STRUCT_MEMBER(aoMapPath)
	REFLECT_STRUCT_MEMBER(gAlbedo)
	REFLECT_STRUCT_MEMBER(gNormal)
	REFLECT_STRUCT_MEMBER(HaveNormal)
	REFLECT_STRUCT_MEMBER(gMetallic)
	REFLECT_STRUCT_MEMBER(gRoughness)
	REFLECT_STRUCT_MEMBER(gAo)
	REFLECT_STRUCT_MEMBER(ClearCoat)
	REFLECT_STRUCT_MEMBER(ClearCoatRoughness)
	REFLECT_STRUCT_END()

	SubMaterial::SubMaterial(const std::string& AlbedoMapPath, const std::string& NormalMapPath, const std::string& MetallicMapPath, const std::string& RoughnessMapPath, const std::string& AoMapPath, bool haveNormal)
	{
		albedoMap = Texture2D::Create(AlbedoMapPath);//directly use physical path
		normalMap = Texture2D::Create(NormalMapPath);
		metallicMap = Texture2D::Create(MetallicMapPath);
		roughnessMap = Texture2D::Create(RoughnessMapPath);
		aoMap = Texture2D::Create(AoMapPath);

		//dont decord these physical path

		HaveNormal = haveNormal;
	}

	void SubMaterial::PostLoad()
	{
		if (!albedoMapPath.empty())
			albedoMap = AssetManager::GetSingleton().GetTexture(albedoMapPath);
		if (!normalMapPath.empty())
			normalMap = AssetManager::GetSingleton().GetTexture(normalMapPath);
		if (!metallicMapPath.empty())
			metallicMap = AssetManager::GetSingleton().GetTexture(metallicMapPath);
		if (!roughnessMapPath.empty())
			roughnessMap = AssetManager::GetSingleton().GetTexture(roughnessMapPath);
		if (!aoMapPath.empty())
			aoMap = AssetManager::GetSingleton().GetTexture(aoMapPath);
	}

	void MaterialComponent::AddMaterial()
	{
		//from the asset manager to get the sub material
		m_Materials.push_back(nullptr);
		m_MaterialPaths.push_back(std::string());
	}

	void MaterialComponent::PostLoad()
	{
		//from the m_MaterialPaths to load sub material
		m_Materials.resize(m_MaterialPaths.size());

		for (size_t i = 0; i < m_Materials.size(); ++i)
		{
			m_Materials[i] = AssetManager::GetSingleton().GetMaterial(m_MaterialPaths[i]);
		}
	}

	REFLECT_STRUCT_BEGIN(MaterialComponent)
	//REFLECT_STRUCT_MEMBER(m_Materials)
	REFLECT_STRUCT_MEMBER(m_MaterialPaths)
	REFLECT_STRUCT_END()

}