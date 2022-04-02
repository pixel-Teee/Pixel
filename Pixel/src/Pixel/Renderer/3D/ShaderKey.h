#pragma once

#include <string>
#include <map>

namespace Pixel {
	struct MaterialShaderPara;
	class ShaderKey
	{
	public:
		ShaderKey();
		~ShaderKey();

		//find whether m_KeyMap have the (Name, uiKeyId)
		bool IsHaveTheKey(const std::string& Name, uint32_t uiKeyId);
		//add or set the key
		void SetTheKey(const std::string& Name, uint32_t uiKeyId);

		//Create ShaderKey, in terms of MSPara and PassType
		static void SetMaterialShaderKey(Ref<ShaderKey> pKey, MaterialShaderPara& MSPara, uint32_t uiPassType);

	protected:
		std::map<std::string, uint32_t> m_KeyMap;
		friend bool operator==(const ShaderKey& Key1, const ShaderKey& Key2);
		//friend bool operator>(const ShaderKey& Key1, const ShaderKey& Key2);
		friend bool operator<(const ShaderKey& Key1, const ShaderKey& Key2);

		void operator=(const ShaderKey& shaderKey);
		//TODO: implement the GetDefine, get the KeyMap to transform the D3DMacro
		//void GetDefine()
		void Clear();
	};
}
