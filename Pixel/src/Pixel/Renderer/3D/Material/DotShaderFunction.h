#pragma once

#include "ShaderFunction.h"

namespace Pixel {
	class Meta(Enable) DotShaderFunction : public ShaderFunction
	{
	public:
		Meta()
		DotShaderFunction();

		DotShaderFunction(const std::string & showName, Ref<Material> pMaterial);

		virtual ~DotShaderFunction();

		virtual bool GetFunctionString(std::string & OutString) const override;

		virtual bool ResetValueType() const override;

		virtual void ConstructPutNodeAndSetPutNodeOwner();

		RTTR_ENABLE(ShaderFunction)
		RTTR_REGISTRATION_FRIEND
	};
}