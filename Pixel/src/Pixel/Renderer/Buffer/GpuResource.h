#pragma once

#include "Pixel/Renderer/RendererType.h"

namespace Pixel {
	class DescriptorCpuHandle;
	class GpuResource
	{
	public:
		virtual ~GpuResource();
		
		virtual void Destroy() = 0;

		virtual void SetResource(void* resource) = 0;

		//------Pixel Buffer Interface------
		//virtual void CreateFromSwapChain(Ref<GpuResource> pResource, const std::wstring& Name) = 0;
		//------Pixel Buffer Interface------

		//------Gpu Buffer Interface------
		virtual Ref<DescriptorCpuHandle> GetUAV() const = 0;
		virtual Ref<DescriptorCpuHandle> GetSRV() const = 0;
		//------Gpu Buffer Interface------

		static Ref<GpuResource> Create();

		static Ref<GpuResource> Create(ResourceStates CurrentState);

		static Ref<GpuResource> CreateColorBuffer();
	};
}
