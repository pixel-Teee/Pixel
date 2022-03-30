#include "pxpch.h"
#include "ShaderPbrFunction.h"

namespace Pixel {

	ShaderPbrFunction::ShaderPbrFunction()
	{

	}

	ShaderPbrFunction::~ShaderPbrFunction()
	{

	}

	bool ShaderPbrFunction::IsHaveOutput()
	{
		return true;
	}

	bool ShaderPbrFunction::GetInputValueString(std::string& OutString, uint32_t uiOutPutStringType)
	{
		return true;
	}

	bool ShaderPbrFunction::GetInputValueString(std::string& OutString) const
	{
		return true;
	}

	bool ShaderPbrFunction::GetOutputValueString(std::string& OutString) const
	{
		return true;
	}

	bool ShaderPbrFunction::GetFunctionString(std::string& OutString) const
	{
		return true;
	}

	bool ShaderPbrFunction::GetShaderTreeString(std::string& OutString, uint32_t uiOutPutStringType)
	{
		return true;
	}

}