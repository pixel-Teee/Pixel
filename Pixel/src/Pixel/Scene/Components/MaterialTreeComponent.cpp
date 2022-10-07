#include "pxpch.h"

#include "MaterialTreeComponent.h"

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
}