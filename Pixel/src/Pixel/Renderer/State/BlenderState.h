#pragma once

#include "Pixel/Renderer/RendererType.h"

namespace Pixel {
	class BlenderState {
	public:
		virtual ~BlenderState();

		virtual void SetBlendState(BlenderStateType type) = 0;

		static Ref<BlenderState> Create();
	};
}
