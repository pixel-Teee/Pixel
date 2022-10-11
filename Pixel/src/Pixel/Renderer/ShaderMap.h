#pragma once
#include <map>
#include "ShaderKey.h"

namespace Pixel {
	class Shader;//reference shader
	using ShaderSet = std::map<ShaderKey, Ref<Shader>>;
	class ShaderMap//asset manager need to manager this
	{
	public:
		//shader key is a series macro name
		ShaderMap();

		~ShaderMap();

		void SetShader(const std::string& name, const ShaderKey& key, Ref<Shader> pShader);

		Ref<ShaderSet> GetShader(const std::string& Name);

		void DeleteShader(const std::string& name);

		Ref<Shader> GetShader(const std::string& name, const ShaderKey& key);

		void DeleteShader(const std::string& name, const ShaderKey& key);
	private:
		std::map<std::string, Ref<ShaderSet>> m_ShaderMap;//filename and it's ShaderSet
	};
}