#include "PreCompiled.h"

#include "Header/ReflectionOptions.h"
#include "Header/ReflectionParser.h"

int main(int argc, char* argv[])
{
	//auto currentPath = std::filesystem::current_path();

	//std::cout << currentPath.string() << std::endl;

	auto exeDir = std::filesystem::path(argv[0]).parent_path();//Templates will be there

	//std::cout << exeDir.string() << std::endl;

	if (!exeDir.empty())
		std::filesystem::current_path(exeDir);

	auto start = std::chrono::system_clock::now();

	//------from the command argument line to get file name------
	//std::cout << argv[0] << std::endl; argv[0] is executable's full path
	//std::cout << argc << std::endl; argc is 1
	ReflectionOptions options;

	if (argv[1] == nullptr || argv[2] == nullptr || argv[3] == nullptr || argv[4] == nullptr
		|| argv[5] == nullptr || argv[6] == nullptr || argv[7] == nullptr || argv[8] == nullptr)
	{
		std::cout << "command line argument is error!" << std::endl;
	}
	else
	{
		options.targetName = argv[1];
		options.SourceRoot = argv[2];
		options.InputSourceFile = argv[3];
		options.ModuleHeaderFile = argv[4];
		options.OutputModuleSource = argv[5];
		//options.TemplateDirectory = argv[6];
		options.OutputModuleFileDirectory = argv[6];//output generated file directory
		options.PreCompiledHeader = argv[8];

		options.TemplateDirectory = "Templates/";

		std::cout << "targetName:" << argv[1] << std::endl;
		std::cout << "SourceRoot:" << argv[2] << std::endl;
		std::cout << "InputSourceFile:" << argv[3] << std::endl;
		std::cout << "ModuleHeaderFile:" << argv[4] << std::endl;
		std::cout << "OutputModuleSource:" << argv[5] << std::endl;
		//std::cout << "TemplateDirectory:" << argv[6] << std::endl;
		std::cout << "OutputModuleFileDirectory:" << argv[6] << std::endl;

		//------from the includes file to get the -I parameter------
		options.Arguments = 
		{ 
			{
				"-x",
				"c++",
				"-std=c++17"
			}
		};
		std::string includes(argv[7]);
		std::ifstream includesFile(includes);
		std::string include;

		//std::string systemInclude = "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.29.30133/include";

		//options.Arguments.emplace_back("-I" + systemInclude);

		while (std::getline(includesFile, include))
		{
			options.Arguments.emplace_back("-I" + include);
			//std::cout << "-I" << include << std::endl;
		}
		//------from the includes file to get the -I parameter------

		options.Arguments.emplace_back("-D__REFLECTION_PARSER__");
	}

	try
	{
		ReflectionParser parser(options);
		parser.Parse();
		parser.GenerateFiles();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	//------from the command argument line to get file name------

	//------calculate time------
	auto duration = std::chrono::system_clock::now() - start;

	std::cout << "completed in "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;
	//------calculate time------
	return 0;
}