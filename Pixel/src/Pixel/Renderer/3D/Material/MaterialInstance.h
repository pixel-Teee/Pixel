#pragma once

namespace Pixel {
	class CustomFloatValue;
	class CustomTexture2D;
	class Material;
	class Texture2D;
	class Meta(Enable) MaterialInstance
	{
	public:
		Meta()
		MaterialInstance();

		virtual ~MaterialInstance();

		void SetPixelShaderValue(const std::string& name, void* fValue, uint32_t count);

		void SetPixelShaderTexture(const std::string& name, Ref<Texture2D> pTexture);

		void SetMaterial(std::string & virtualPath);//from the asset manager to get the material

	private:

		Meta()
		std::vector<Ref<CustomFloatValue>> m_PSShaderCustomValue;
		Meta()
		std::vector<Ref<CustomTexture2D>> m_PSShaderCustomTexture;

		Ref<Material> m_pMaterial;

		Meta()
		//material virtual path
		std::string m_MaterialPath;

		RTTR_ENABLE()
		RTTR_REGISTRATION_FRIEND
	};
}