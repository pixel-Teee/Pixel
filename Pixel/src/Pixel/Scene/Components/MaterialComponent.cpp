#include "pxpch.h"

#include "MaterialComponent.h"

#include "Pixel/Renderer/Descriptor/DescriptorHeap.h"

namespace Pixel
{
	
	SubMaterial::SubMaterial()
	{
		uint32_t DefaultAlbedoMapTextureData[4] = { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff };

		uint32_t DefaultNormalMapTextureData[4] = { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff };

		uint8_t DefaultMetallicMapTextureData, DefaultRoughnessMapTextureData = 0xff;
		uint8_t DefaultAoMapTextureData = 0;
		//------create texture------
		albedoMap = Texture2D::Create(8, 2, 2, ImageFormat::PX_FORMAT_R8G8B8A8_UNORM, DefaultAlbedoMapTextureData);
		normalMap = Texture2D::Create(8, 2, 2, ImageFormat::PX_FORMAT_R8G8B8A8_UNORM, DefaultNormalMapTextureData);
		metallicMap = Texture2D::Create(1, 1, 1, ImageFormat::PX_FORMAT_R8_UNORM, &DefaultMetallicMapTextureData);
		roughnessMap = Texture2D::Create(1, 1, 1, ImageFormat::PX_FORMAT_R8_UNORM, &DefaultRoughnessMapTextureData);
		aoMap = Texture2D::Create(1, 1, 1, ImageFormat::PX_FORMAT_R8_UNORM, &DefaultAoMapTextureData);
		//------create texture------

		for (uint32_t i = 0; i < 5; ++i)
			nextFrameNeedLoadTexture[i] = false;

		m_pDescriptorHeap = DescriptorHeap::Create(L"Material Descriptor Heap", DescriptorHeapType::CBV_UAV_SRV, 5);
		m_pTextureFirstHandle = m_pDescriptorHeap->Alloc(5);
	}

	RTTR_REGISTRATION
	{
		using namespace rttr;
		registration::class_<SubMaterial>("SubMaterial")
			.constructor<>()
			.property("shadingModel", &SubMaterial::shadingModel)
			.property("albedoMapPath", &SubMaterial::albedoMapPath)
			.property("normalMapPath", &SubMaterial::normalMapPath)
			.property("metallicMapPath", &SubMaterial::metallicMapPath)
			.property("roughnessMapPath", &SubMaterial::roughnessMapPath)
			.property("aoMapPath", &SubMaterial::aoMapPath)
			.property("gAlbedo", &SubMaterial::gAlbedo)
			.property("gNormal", &SubMaterial::gNormal)
			.property("HaveNormal", &SubMaterial::HaveNormal)
			.property("gMetallic", &SubMaterial::gMetallic)
			.property("gRoughness", &SubMaterial::gRoughness)
			.property("gAo", &SubMaterial::gAo)
			.property("ClearCoat", &SubMaterial::ClearCoat)
			.property("ClearCoatRoughness", &SubMaterial::ClearCoatRoughness)
			.property("IsTransparent", &SubMaterial::IsTransparent);
	}

	SubMaterial::SubMaterial(const std::string& AlbedoMapPath, const std::string& NormalMapPath, const std::string& MetallicMapPath, const std::string& RoughnessMapPath, const std::string& AoMapPath, bool haveNormal)
	{
		albedoMap = Texture2D::Create(AlbedoMapPath);//directly use physical path
		normalMap = Texture2D::Create(NormalMapPath);
		metallicMap = Texture2D::Create(MetallicMapPath);
		roughnessMap = Texture2D::Create(RoughnessMapPath);
		aoMap = Texture2D::Create(AoMapPath);

		//dont decord these physical path

		HaveNormal = haveNormal;

		m_pDescriptorHeap = DescriptorHeap::Create(L"Material Descriptor Heap", DescriptorHeapType::CBV_UAV_SRV, 5);
		m_pTextureFirstHandle = m_pDescriptorHeap->Alloc(5);
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
}