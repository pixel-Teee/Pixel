#pragma once

#include "Pixel/Renderer/RendererType.h"

namespace Pixel {
	class BlenderState {
	public:
		virtual ~BlenderState();

		virtual void SetBlendState(BlenderStateType type) = 0;

		virtual void SetRenderTargetBlendState(uint32_t index, bool enableBlend) = 0;

		virtual void SetIndependentBlendEnable(bool enable) = 0;

		static Ref<BlenderState> Create();
	};
}
