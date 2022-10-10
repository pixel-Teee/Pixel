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

	private:
		std::map<std::string, uint32_t> m_KeyMap;
	};
}