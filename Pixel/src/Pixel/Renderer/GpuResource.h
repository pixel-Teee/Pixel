#pragma once

namespace Pixel {

	class GpuResource
	{
	public:
		virtual ~GpuResource();
		
		virtual void Destroy() = 0;

		Ref<GpuResource> Create();
	};
}
