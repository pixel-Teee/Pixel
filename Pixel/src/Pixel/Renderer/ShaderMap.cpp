#include "pxpch.h"

#include "ShaderMap.h"

namespace Pixel {
	ShaderMap::ShaderMap()
	{

	}

	ShaderMap::~ShaderMap()
	{
		m_ShaderMap.clear();//clear shader map
	}

	void ShaderMap::SetShader(const std::string& name, const ShaderKey& key, Ref<Shader> pShader)
	{
		//first find shader set, then find shader key, then assign pShader
		m_ShaderMap[name][key] = pShader;
	}

	ShaderMap::ShaderSet& ShaderMap::GetShader(const std::string& Name)
	{
		return m_ShaderMap[Name];
	}

	void ShaderMap::DeleteShader(const std::string& name)
	{
		auto iter = m_ShaderMap.find(name);
		if (iter != m_ShaderMap.end())
		{
			m_ShaderMap.erase(iter);//erase iter
		}
	}
	Ref<Shader> ShaderMap::GetShader(const std::string& name, const ShaderKey& key)
	{
		if (m_ShaderMap.find(name) != m_ShaderMap.end())
		{
			if (m_ShaderMap[name].find(key) != m_ShaderMap[name].end())
			{
				return m_ShaderMap[name][key];
			}
			return nullptr;
		}
		return nullptr;
	}
	void ShaderMap::DeleteShader(const std::string& name, const ShaderKey& key)
	{
		auto iter = m_ShaderMap.find(name);//return std::set
		if (iter != m_ShaderMap.end())
		{
			auto iter2 = iter->second.find(key);
			if (iter2 != iter->second.end())
			{
				iter->second.erase(iter2);//erase it
			}
		}
	}
}