#pragma once

namespace Pixel {
	struct SubMaterial;
	struct MaterialComponent
	{
		std::vector<Ref<SubMaterial>> m_Materials;
		std::vector<std::string> m_MaterialPaths;//material's virtual path

		MaterialComponent() {}
		MaterialComponent(const MaterialComponent&) = default;

		void AddMaterial();

		void PostLoad();//load mutiple materials

		RTTR_ENABLE()
		RTTR_REGISTRATION_FRIEND
	};
}