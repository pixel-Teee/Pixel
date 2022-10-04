#pragma once

namespace Pixel {
	class MaterialInstance;
	struct Meta(Enable) MaterialTreeComponent
	{
		Meta()
		MaterialTreeComponent();

		std::vector<Ref<MaterialInstance>> m_Materials;

		Meta()
		std::vector<std::string> m_MaterialPaths;

		void AddMaterialInstance();

		RTTR_ENABLE()
		RTTR_REGISTRATION_FRIEND
	};
}