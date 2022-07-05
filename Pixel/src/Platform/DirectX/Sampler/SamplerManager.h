#pragma once

#include "Platform/DirectX/d3dx12.h"
#include <glm/glm.hpp>

#include "Pixel/Renderer/Sampler/Sampler.h"

namespace Pixel {
	class Device;
	class DirectXSamplerDesc : public SamplerDesc {
	public:
		//these defaults match the default values for hlsl-defined root signature static samplers
		//so not overriding them here means you can safely not define then in hlsl
		DirectXSamplerDesc();

		virtual void SetTextureAddressMode(AddressMode addressMode) override;

		virtual void SetFilter(Filter filter) override;

		void SetBoarderColor(glm::vec4 BorderColor);

		//allocate new descriptor as needed
		//return handle to existing descriptor when possible
		D3D12_CPU_DESCRIPTOR_HANDLE CreateDescriptor();

		D3D12_SAMPLER_DESC m_SamplerDesc;

		//static Ref<SamplerDesc> Create();
	};
}
