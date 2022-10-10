
#include <map>
#include "ShaderKey.h"

namespace Pixel {
	class Shader;//reference shader
	class ShaderMap//asset manager need to manager this
	{
	public:
		using ShaderSet = std::map<ShaderKey, Ref<Shader>>;

		ShaderMap();

		~ShaderMap();

		void SetShader(const std::string& name, const ShaderKey& key, Ref<Shader> pShader);

		ShaderSet& GetShader(const std::string& Name);

		void DeleteShader(const std::string& name);

		Ref<Shader> GetShader(const std::string& name, const ShaderKey& key);

		void DeleteShader(const std::string& name, const ShaderKey& key);
	private:
		std::map<std::string, ShaderSet> m_ShaderMap;//filename and it's ShaderSet
	};
}