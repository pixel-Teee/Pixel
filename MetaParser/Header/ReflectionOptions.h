#pragma once

struct ReflectionOptions
{
	bool displayDiagnostics = true;//display error message

	std::string targetName;//generated reflection module target, will be compiled to library

	std::string SourceRoot;//header and source's directory

	std::string InputSourceFile;//contains two parsed header file's header file

	std::string ModuleHeaderFile;//in terms of the module header file, to generate the module source file

	std::string OutputModuleSource;//module source, will reference meta file to register
	
	std::string OutputModuleFileDirectory;//output reflection file directory

	std::string TemplateDirectory;//template data file directory

	std::string PreCompiledHeader;

	std::vector<std::string> Arguments;//-I -D
};