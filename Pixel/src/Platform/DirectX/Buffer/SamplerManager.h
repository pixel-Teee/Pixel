#pragma once

#include "Platform/DirectX/d3dx12.h"
#include <glm/glm.hpp>

namespace Pixel {
	class Device;
	class SamplerDesc {
		//these defaults match the default values for hlsl-defined root signature static samplers
		//so not overriding them here means you can safely not define then in hlsl
		SamplerDesc();

		void SetTextureAddressMode(D3D12_TEXTURE_ADDRESS_MODE AddressMode);

		void SetBoarderColor(glm::vec4 BorderColor);

		//allocate new descriptor as needed
		//return handle to existing descriptor when possible
		D3D12_CPU_DESCRIPTOR_HANDLE CreateDescriptor(Ref<Device> pDevice);

		D3D12_SAMPLER_DESC m_SamplerDesc;
	};
}
