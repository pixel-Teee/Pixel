#include "pxpch.h"

#include "ShaderKey.h"

namespace Pixel {
	ShaderKey::ShaderKey()
	{
	}

	ShaderKey::~ShaderKey()
	{
	}

	bool ShaderKey::HaveShaderKey(const std::string& macroName)
	{
		return m_KeyMap.find(macroName) != m_KeyMap.end();
	}

	void ShaderKey::SetShaderKey(const std::string& macroName, uint32_t value)
	{
		m_KeyMap[macroName] = value;
	}

	std::map<std::string, uint32_t>& ShaderKey::GetShaderMap()
	{
		return m_KeyMap;
	}

	bool ShaderKey::operator!=(const ShaderKey& rhs) const
	{
		return !operator==(rhs);
	}

	bool ShaderKey::operator==(const ShaderKey& rhs) const
	{
		if (m_KeyMap.size() != rhs.m_KeyMap.size())
			return false;

		for (auto& item : m_KeyMap)
		{
			auto iter = rhs.m_KeyMap.find(item.first);
			if (iter == rhs.m_KeyMap.end())
				return false;
			
			uint32_t value = iter->second;

			if (item.second != value)
				return false;//return false
		}
		return true;
	}

	bool ShaderKey::operator<(const ShaderKey& rhs) const
	{
		if (m_KeyMap.size() < rhs.m_KeyMap.size())
			return true;
		else
			return false;

		//compare key
		for (auto& item : m_KeyMap)
		{
			for (auto& item2 : rhs.m_KeyMap)
			{
				if (item < item2)
					return true;
				else
					return false;
			}
		}
		//compare value
		for (auto& item : m_KeyMap)
		{
			for (auto& item2 : rhs.m_KeyMap)
			{
				if (item.second < item2.second)
					return true;
				else
					return false;
			}
		}
		return false;
	}

	bool ShaderKey::operator>(const ShaderKey& rhs) const
	{
		if (m_KeyMap.size() > rhs.m_KeyMap.size())
			return true;
		else
			return false;

		//compare key
		for (auto& item : m_KeyMap)
		{
			for (auto& item2 : rhs.m_KeyMap)
			{
				if (item > item2)
					return true;
				else
					return false;
			}
		}
		//compare value
		for (auto& item : m_KeyMap)
		{
			for (auto& item2 : rhs.m_KeyMap)
			{
				if (item.second > item2.second)
					return true;
				else
					return false;
			}
		}
		return false;
	}

}