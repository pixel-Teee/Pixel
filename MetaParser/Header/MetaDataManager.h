#pragma once

#include "Mustache/mustache.hpp"
#include "Cursor.h"

class ReflectionParser;

class MetaDataManager
{
public:
	MetaDataManager(const Cursor& cursor);

	//get key's value
	std::string GetProperty(const std::string& key) const;

	//have this key?
	bool GetFlag(const std::string& key) const;

	std::string GetNativeString(const std::string& key) const;

	void CompileTemplateData(kainjow::mustache::data& data, const ReflectionParser* context) const;
private:
	//key value
	using Property = std::pair<std::string, std::string>;

	std::unordered_map<std::string, std::string> m_Properties;

	//from the attribute annotate to extract the properties
	std::vector<Property> extractProperties(const Cursor& cursor) const;
};