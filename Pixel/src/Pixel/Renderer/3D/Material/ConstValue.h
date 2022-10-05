#pragma once

#include "ShaderFunction.h"

namespace Pixel
{
	//user provdied, will link to a UserConstant(just to assignment to real shader value) class
	class Meta(Enable) ConstValue : public ShaderFunction
	{
	public:
		//Meta()
		ConstValue();
		//if bIsCustom, then will declare as uniform variable
		ConstValue(const std::string& showName, Ref<Material> pMaterial, uint32_t valueNumber, bool bIsCustom);

		virtual bool GetDeclareString(std::string & OutString, int32_t registerId = -1) const = 0;

		virtual ~ConstValue();

		Meta()
		//if this is not user provided, then use default value
		bool m_bIsCustom;

		RTTR_ENABLE(ShaderFunction)
		RTTR_REGISTRATION_FRIEND
	};
}