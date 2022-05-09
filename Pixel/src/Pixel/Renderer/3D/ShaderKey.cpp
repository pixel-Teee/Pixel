#include "pxpch.h"
#include "ShaderKey.h"
#include "Pixel/Renderer/3D/RendererPass.h"

namespace Pixel {

	ShaderKey::ShaderKey()
	{

	}

	ShaderKey::~ShaderKey()
	{

	}

	bool ShaderKey::IsHaveTheKey(const std::string& Name, uint32_t uiKeyId)
	{
		auto it = m_KeyMap.find(Name);
		if (it->second != uiKeyId) return false;
		return true;
	}

	void ShaderKey::SetTheKey(const std::string& Name, uint32_t uiKeyId)
	{
		m_KeyMap[Name] = uiKeyId;
	}

	void ShaderKey::SetMaterialShaderKey(Ref<ShaderKey> pKey, MaterialShaderPara& MSPara, uint32_t uiPassType)
	{
		if (!pKey) return;

		if (uiPassType == RenderPass::PT_GEOMETRY)
		{
			pKey->SetTheKey("GeometryPass", 1);
		}
	}

	void ShaderKey::Clear()
	{
		m_KeyMap.clear();
	}

	void ShaderKey::operator=(const ShaderKey& shaderKey)
	{
		m_KeyMap = shaderKey.m_KeyMap;
	}

	bool operator==(const ShaderKey& Key1, const ShaderKey& Key2)
	{
		if (Key1.m_KeyMap.size() != Key2.m_KeyMap.size())
		{
			return false;
		}

		for (auto element : Key1.m_KeyMap)
		{
			auto it = Key2.m_KeyMap.find(element.first);

			if (it == Key2.m_KeyMap.end())
			{
				return false;
			}

			if (element.second != it->second)
			{
				return false;
			}
		}
		return true;
	}

	bool operator<(const ShaderKey& Key1, const ShaderKey& Key2)
	{
		if (Key1.m_KeyMap.size() != Key2.m_KeyMap.size())
		{
			return false;
		}

		std::vector<std::string> s1;
		std::vector<std::string> s2;

		std::vector<uint32_t> v1;
		std::vector<uint32_t> v2;

		for (auto element : Key1.m_KeyMap)
		{
			s1.push_back(element.first);
			v1.push_back(element.second);
		}

		for (auto element : Key2.m_KeyMap)
		{
			s1.push_back(element.first);
			v1.push_back(element.second);
		}

		for (uint32_t i = 0; i < s1.size(); ++i)
		{
			if (s1[i] < s2[i]) return true;
			else if (s1[i] > s2[i]) return false;
		}

		for (uint32_t i = 0; i < s1.size(); ++i)
		{
			if (v1[i] < v2[i]) return true;
			else if (v1[i] > v2[i]) return false;
		}
		return false;
	}

}