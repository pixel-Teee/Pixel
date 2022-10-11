#pragma once

#include <map>
#include <string>

namespace Pixel {
	//shader key is some shader macro
	class ShaderKey
	{
	public:
		ShaderKey();

		~ShaderKey();

		bool HaveShaderKey(const std::string& macroName);

		void SetShaderKey(const std::string& macroName, uint32_t value);

		bool operator!=(const ShaderKey& rhs) const;

		bool operator==(const ShaderKey& rhs) const;

		bool operator<(const ShaderKey& rhs) const;

		bool operator>(const ShaderKey& rhs) const;
	private:
		//macro name and it's value
		std::map<std::string, uint32_t> m_KeyMap;
	};
}