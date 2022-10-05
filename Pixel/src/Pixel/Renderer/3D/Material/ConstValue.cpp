#include "pxpch.h"

#include "ConstValue.h"

#include "ShaderFunction.h"
#include "ShaderFunction.h"

namespace Pixel
{
	ConstValue::ConstValue()
	{
		m_bIsCustom = false;
	}

	//------ConstValue------
	ConstValue::ConstValue(const std::string& showName, Ref<Material> pMaterial, uint32_t valueNumber, bool bIsCustom)
		:ShaderFunction(showName, pMaterial)
	{
		m_bIsCustom = bIsCustom;
		//m_functionType = ShaderFunction::ShaderFunctionType::ConstFloat4;
	}

	ConstValue::~ConstValue()
	{

	}
	//------ConstValue------
}
