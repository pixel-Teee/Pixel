#pragma once

#include "Pixel/Renderer/RendererType.h"

namespace Pixel {

	class GpuResource
	{
	public:
		virtual ~GpuResource();
		
		virtual void Destroy() = 0;

		virtual void SetResource(void* resource) = 0;

		static Ref<GpuResource> Create();

		static Ref<GpuResource> Create(ResourceStates CurrentState);
	};
}
