#pragma once

#include "Pixel/Renderer/3D/Material.h"

namespace Pixel {
	//---experimental component---
	struct MaterialTreeComponent {
		std::string path;//material tree component file path
		Ref<Material> m_pMaterial;
		Ref<MaterialInstance> m_pMaterialInstance;
		MaterialTreeComponent(const std::string& Path) {
			path = Path;
			//from this path to load the material file's logic node
			//there add one shader main function
			m_pMaterial = CreateRef<Material>("Material", Material::MUT_GEO);
			m_pMaterialInstance = CreateRef<MaterialInstance>(m_pMaterial);
		}
	};
	//---experimental component---
}