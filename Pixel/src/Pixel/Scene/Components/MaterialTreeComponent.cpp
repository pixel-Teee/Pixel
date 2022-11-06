#include "pxpch.h"

#include "MaterialTreeComponent.h"

#include "Pixel/Asset/AssetManager.h"

namespace Pixel {
	MaterialTreeComponent::MaterialTreeComponent()
	{

	}

	MaterialTreeComponent::MaterialTreeComponent(const MaterialTreeComponent& rhs)
	{
		m_MaterialPaths = rhs.m_MaterialPaths;
		m_Materials = rhs.m_Materials;//directly use reference
	}

	void MaterialTreeComponent::AddMaterialInstance()
	{
		m_Materials.push_back(nullptr);

		m_MaterialPaths.push_back(std::string());
	}

	void MaterialTreeComponent::PostLoad()
	{
		m_Materials.resize(m_MaterialPaths.size());
		for (size_t i = 0; i < m_MaterialPaths.size(); ++i)
		{
			m_Materials[i] = AssetManager::GetSingleton().GetMaterialInstance(m_MaterialPaths[i]);
		}
	}
}