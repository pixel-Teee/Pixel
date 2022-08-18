#pragma once

#include "ShaderFunction.h"

namespace Pixel
{
	//user provdied, will link to a UserConstant(just to assignment to real shader value) class
	class ConstValue : public ShaderFunction
	{
	public:
		//if bIsCustom, then will declare as uniform variable
		ConstValue(const std::string& showName, Ref<Material> pMaterial, uint32_t valueNumber, bool bIsCustom);

		virtual ~ConstValue();

		//If this is not user provided, then use default value
		bool m_bIsCustom;
	};
}