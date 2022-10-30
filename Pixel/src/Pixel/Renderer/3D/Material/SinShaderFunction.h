#pragma once

#include "ShaderFunction.h"

namespace Pixel {
	class Meta(Enable) SinShaderFunction : public ShaderFunction
	{
	public:
		Meta()
		SinShaderFunction();

		SinShaderFunction(const std::string & showName, Ref<Material> pMaterial);

		virtual ~SinShaderFunction();

		virtual bool GetFunctionString(std::string & OutString) const override;

		//virtual bool ResetValueType() const override;

		virtual void ConstructPutNodeAndSetPutNodeOwner();

		RTTR_ENABLE(ShaderFunction)
		RTTR_REGISTRATION_FRIEND
	};
}