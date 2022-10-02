#pragma once

#include "ShaderFunction.h"

namespace Pixel {
	class Meta(Enable) TextureShaderFunction : public ShaderFunction
	{
	public:
		Meta()
		TextureShaderFunction();

		TextureShaderFunction(const std::string & ShowName, Ref<Material> pMaterial);

		virtual ~TextureShaderFunction();

		virtual bool GetInputValueString(std::string& OutString) const;

		virtual bool GetDeclareString(std::string& OutString, int32_t registerId = -1) const;

		//virtual bool GetOutputValueString(std::string& OutString) const = 0;

	protected:
		bool m_bDecodeNormal;

		RTTR_ENABLE(ShaderFunction)
		RTTR_REGISTRATION_FRIEND
	};
}