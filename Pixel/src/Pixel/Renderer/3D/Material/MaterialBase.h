#pragma once

#include "MaterialInterface.h"
#include "CustomFloatValue.h"
#include "CustomSamplerState.h"

namespace Pixel {
	class MaterialBase : public MaterialInterface
	{
	public:
		//vertex shader, vertex shader will be rid
		//TODO:our renderer is deferred shading
		std::vector<Ref<CustomFloatValue>> m_VSShaderCustomValue;
		std::vector<Ref<CustomSamplerState>> m_VSShaderCustomSampler;

		//pixel shader
		std::vector<Ref<CustomFloatValue>> m_PSShaderCustomValue;
		std::vector<Ref<CustomSamplerState>> m_PSShaderCustomSampler;
	};
}