#pragma once

#include "ShaderFunction.h"

namespace Pixel {

	class Meta(Enable) ComponentMask : public ShaderFunction
	{
	public:
		Meta()
		ComponentMask();

		ComponentMask(const std::string & showName, Ref<Material> pMaterial);

		virtual ~ComponentMask();

		virtual bool GetFunctionString(std::string & OutString) const override;

		//virtual bool ResetValueType() const override;

		virtual void ConstructPutNodeAndSetPutNodeOwner();

		void SetMask(uint32_t maskIndex, bool enable);

		void UpdateOutputNodeValueType();

		//mask, could modify, from editor
		Meta()
		bool R;
		Meta()
		bool G;
		Meta()
		bool B;
		Meta()
		bool A;

		RTTR_ENABLE(ShaderFunction)
		RTTR_REGISTRATION_FRIEND
	};
}