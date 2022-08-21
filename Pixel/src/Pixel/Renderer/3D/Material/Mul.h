#pragma once

#include "ShaderFunction.h"

namespace Pixel {
	class Mul : public ShaderFunction
	{
	public:
		Mul();

		Mul(const std::string& showName, Ref<Material> pMaterial);

		virtual ~Mul();

		virtual bool GetFunctionString(std::string& OutString) const override;

		virtual bool ResetValueType() const override;

		virtual void ConstructPutNodeAndSetPutNodeOwner();

		REFLECT()
	};
}