#pragma once

#include "ShaderFunction.h"

namespace Pixel {
	class Meta(Enable) Mul : public ShaderFunction
	{
	public:
		Meta()
		Mul();

		Mul(const std::string& showName, Ref<Material> pMaterial);

		virtual ~Mul();

		virtual bool GetFunctionString(std::string& OutString) const override;

		virtual bool ResetValueType() const override;

		virtual void ConstructPutNodeAndSetPutNodeOwner();

		RTTR_ENABLE(ShaderFunction)
		RTTR_REGISTRATION_FRIEND
	};
}