#pragma once

#include "Pixel/Renderer/3D/Material.h"

namespace Pixel {
	class SerializerMaterial {
	public:
		void SerializerMaterialAsset(std::string& filePath, Ref<Material> pMaterial, Ref<MaterialInstance> pMaterialInstance);

		void DeserializerMaterialAssetAndCreateMaterial(const std::string& filepath, Ref<Material>& pOutMaterial
		,Ref<MaterialInstance>& pOutMaterialInstance);
	//public:
		//Ref<Material> m_pMaterial;
		//Ref<MaterialInstance> m_pMaterialInstance;
	};
}