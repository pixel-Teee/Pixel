#pragma once

#include "CustomFloatValue.h"
#include "CustomTexture2D.h"

namespace Pixel {
	class Meta(Enable) MaterialBase
	{
	public:
		Meta()
		MaterialBase();
		//vertex shader, vertex shader will be rid
		//TODO:our renderer is deferred shading
		std::vector<Ref<CustomFloatValue>> m_VSShaderCustomValue;
		std::vector<Ref<CustomTexture2D>> m_VSShaderCustomSampler;

		//pixel shader
		Meta()
		std::vector<Ref<CustomFloatValue>> m_PSShaderCustomValue;
		Meta()
		std::vector<Ref<CustomTexture2D>> m_PSShaderCustomTexture;

		RTTR_ENABLE()
		RTTR_REGISTRATION_FRIEND
	};
}