#pragma once

//------other library------
#include <glm/glm.hpp>
#include "Platform/DirectX/d3dx12.h"
//------other library------

//------my library------
#include "Pixel/Renderer/Sampler/Sampler.h"
//------my library------

namespace Pixel {
	class Device;
	class DirectXSamplerDesc : public SamplerDesc {
	public:
		//these defaults match the default values for hlsl-defined root signature static samplers
		//so not overriding them here means you can safely not define then in hlsl
		DirectXSamplerDesc();

		virtual void SetTextureAddressMode(AddressMode addressMode) override;

		virtual void SetFilter(Filter filter) override;

		//allocate new descriptor as needed
		//return handle to existing descriptor when possible
		D3D12_CPU_DESCRIPTOR_HANDLE CreateDescriptor();

		D3D12_SAMPLER_DESC m_SamplerDesc;

		virtual void SetBorderColor(glm::vec4 BoarderColor) override;

		virtual Ref<SamplerDesc> Clone() override;
	};
}
