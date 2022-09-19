#pragma once

#include "Cursor.h"
#include "Namespace.h"
#include "ModuleFile.h"
#include "ReflectionOptions.h"

#include "Mustache/mustache.hpp"

class ReflectionParser
{
public:
	ReflectionParser(const ReflectionOptions& options);

	~ReflectionParser();

	void Parse();

	void GenerateFiles();

	kainjow::mustache::mustache LoadTemplate(const std::string& name) const;

private:

	ReflectionOptions m_Options;
	
	CXIndex m_Index;

	CXTranslationUnit m_TranslationUnit;

	//header template
	kainjow::mustache::mustache m_ModuleFileHaderTemplate;
	//source template
	kainjow::mustache::mustache m_ModuleFileSourceTemplate;

	//mutable std::unordered_map<std::string, std::string> m_templatePartialCache;

	std::unordered_map<std::string, ModuleFile> m_ModuleFiles;//generated reflection information

	//currentNamespace will save the class's typedef
	void BuildClasses(const Cursor& cursor, Namespace& currentNamespace);

	void BuildEnums(const Cursor& cursor, Namespace& currentNamespace);

	void AddGlobalTemplateData(kainjow::mustache::data& data);

	void GenerateModuleFile(const std::filesystem::path& fileHeader, const std::filesystem::path& fileSource,
		const std::string& SourceHeader, const ModuleFile& file);
};