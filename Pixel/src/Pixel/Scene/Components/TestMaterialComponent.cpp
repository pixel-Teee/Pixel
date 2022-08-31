#include "pxpch.h"

#include "TestMaterialComponent.h"

#include "Pixel/Asset/AssetManager.h"

namespace Pixel {

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

	RTTR_REGISTRATION
	{
		using namespace rttr;
		registration::class_<MaterialComponent>("MaterialComponent")
			.constructor<>()
			.property("m_MaterialPaths", &MaterialComponent::m_MaterialPaths);
	}
}