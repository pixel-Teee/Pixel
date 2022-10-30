
#include "ShaderFunction.h"

namespace Pixel {
	class Meta(Enable) LerpShaderFunction : public ShaderFunction
	{
	public:
		Meta()
		LerpShaderFunction();

		LerpShaderFunction(const std::string & showName, Ref<Material> pMaterial);

		virtual ~LerpShaderFunction();

		virtual bool GetFunctionString(std::string & OutString) const override;

		virtual bool ResetValueType() const override;

		virtual void ConstructPutNodeAndSetPutNodeOwner();

		RTTR_ENABLE(ShaderFunction)
		RTTR_REGISTRATION_FRIEND
	};
}