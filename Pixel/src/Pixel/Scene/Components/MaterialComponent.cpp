#include "pxpch.h"

#include "MaterialComponent.h"

namespace Pixel
{
	
	SubMaterial::SubMaterial()
	{
		uint32_t DefaultAlbedoMapTextureData[4] = { 0xffffff, 0, 0xffffff, 0 };

		uint32_t DefaultNormalMapTextureData[4] = { 0xffffff, 0, 0xffffff, 0 };

		uint32_t DefaultMetallicMapTextureData, DefaultRoughnessMapTextureData, DefaultAoMapTextureData = 0xff;

		//------create texture------
		albedoMap = Texture2D::Create(8, 2, 2, ImageFormat::PX_FORMAT_R8G8B8A8_UNORM, &DefaultAlbedoMapTextureData);
		normalMap = Texture2D::Create(8, 2, 2, ImageFormat::PX_FORMAT_R8G8B8A8_UNORM, &DefaultNormalMapTextureData);
		metallicMap = Texture2D::Create(1, 1, 1, ImageFormat::PX_FORMAT_R8_UNORM, &DefaultMetallicMapTextureData);
		roughnessMap = Texture2D::Create(1, 1, 1, ImageFormat::PX_FORMAT_R8_UNORM, &DefaultRoughnessMapTextureData);
		aoMap = Texture2D::Create(1, 1, 1, ImageFormat::PX_FORMAT_R8_UNORM, &DefaultAoMapTextureData);
		//------create texture------

		for (uint32_t i = 0; i < 5; ++i)
			nextFrameNeedLoadTexture[i] = false;
	}

	REFLECT_STRUCT_BEGIN(SubMaterial)
	//REFLECT_STRUCT_MEMBER(albedoMap)
	//REFLECT_STRUCT_MEMBER(normalMap)
	//REFLECT_STRUCT_MEMBER(metallicMap)
	//REFLECT_STRUCT_MEMBER(roughnessMap)
	//REFLECT_STRUCT_MEMBER(aoMap)
	REFLECT_STRUCT_MEMBER(albedoMapPath)
	REFLECT_STRUCT_MEMBER(normalMapPath)
	REFLECT_STRUCT_MEMBER(roughnessMapPath)
	REFLECT_STRUCT_MEMBER(aoMapPath)
	REFLECT_STRUCT_MEMBER(gAlbedo)
	REFLECT_STRUCT_MEMBER(gNormal)
	REFLECT_STRUCT_MEMBER(HaveNormal)
	REFLECT_STRUCT_MEMBER(gMetallic)
	REFLECT_STRUCT_MEMBER(gRoughness)
	REFLECT_STRUCT_MEMBER(gAo)
	REFLECT_STRUCT_MEMBER(gEmissive)
	REFLECT_STRUCT_MEMBER(ClearCoat)
	REFLECT_STRUCT_MEMBER(ClearCoatRoughness)
	REFLECT_STRUCT_END()

	void SubMaterial::Initialize(const std::string& filePath)
	{
		//filepath is physical path
		rapidjson::Document doc;

		std::ifstream stream(filePath);
		std::stringstream strStream;
		strStream << stream.rdbuf();

		if (!doc.Parse(strStream.str().data()).HasParseError())
		{
			if (doc.HasMember("SubMaterial") && doc["SubMaterial"].IsObject())
			{

			}
		}

		stream.close();
	}

	void MaterialComponent::AddMaterial()
	{
		m_Materials.push_back(SubMaterial());//add default sub material
	}

	void MaterialComponent::PostLoad()
	{

	}

	REFLECT_STRUCT_BEGIN(MaterialComponent)
	//REFLECT_STRUCT_MEMBER(m_Materials)
	REFLECT_STRUCT_MEMBER(m_MaterialPaths)
	REFLECT_STRUCT_END()

}