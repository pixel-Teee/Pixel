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

RTTR_REGISTRATION
{
	using namespace rttr;
	registration::class_<Pixel::SubMaterial>("SubMaterial")
		.constructor<>()
		.property("shadingModel", &Pixel::SubMaterial::shadingModel)
		.property("albedoMapPath", &Pixel::SubMaterial::albedoMapPath)
		.property("normalMapPath", &Pixel::SubMaterial::normalMapPath)
		.property("metallicMapPath", &Pixel::SubMaterial::metallicMapPath)
		.property("roughnessMapPath", &Pixel::SubMaterial::roughnessMapPath)
		.property("aoMapPath", &Pixel::SubMaterial::aoMapPath)
		.property("gAlbedo", &Pixel::SubMaterial::gAlbedo)
		.property("gNormal", &Pixel::SubMaterial::gNormal)
		.property("HaveNormal", &Pixel::SubMaterial::HaveNormal)
		.property("gMetallic", &Pixel::SubMaterial::gMetallic)
		.property("gRoughness", &Pixel::SubMaterial::gRoughness)
		.property("gAo", &Pixel::SubMaterial::gAo)
		.property("ClearCoat", &Pixel::SubMaterial::ClearCoat)
		.property("ClearCoatRoughness", &Pixel::SubMaterial::ClearCoatRoughness)
		.property("IsTransparent", &Pixel::SubMaterial::IsTransparent);

	std::cout << "register sub material successfully!" << std::endl;
}