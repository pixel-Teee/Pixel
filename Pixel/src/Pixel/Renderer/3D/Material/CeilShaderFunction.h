#pragma once

#include "ShaderFunction.h"

namespace Pixel {
	class Meta(Enable) CeilShaderFunction : public ShaderFunction
	{
	public:

		Meta()
		CeilShaderFunction();

		CeilShaderFunction(const std::string & showName, Ref<Material> pMaterial);

		virtual ~CeilShaderFunction();

		virtual bool GetFunctionString(std::string & OutString) const override;

		virtual bool ResetValueType() const override;

		virtual void ConstructPutNodeAndSetPutNodeOwner();

		RTTR_ENABLE(ShaderFunction)
		RTTR_REGISTRATION_FRIEND
	};
}