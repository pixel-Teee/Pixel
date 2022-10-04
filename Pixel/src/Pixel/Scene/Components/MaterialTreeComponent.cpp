#include "pxpch.h"

#include "MaterialTreeComponent.h"

namespace Pixel {
	MaterialTreeComponent::MaterialTreeComponent()
	{

	}

	void MaterialTreeComponent::AddMaterialInstance()
	{
		m_Materials.push_back(nullptr);

		m_MaterialPaths.push_back(std::string());
	}
}