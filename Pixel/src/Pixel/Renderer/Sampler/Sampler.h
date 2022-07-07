#pragma once

#include "Pixel/Renderer/RendererType.h"

namespace Pixel {
	class SamplerDesc {
	public:
		virtual void SetTextureAddressMode(AddressMode addressMode) = 0;

		virtual void SetBorderColor(glm::vec4 BoarderColor) = 0;
		virtual void SetFilter(Filter filter) = 0;
		static Ref<SamplerDesc> Create();
	};
}