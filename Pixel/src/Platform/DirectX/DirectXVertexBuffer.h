#pragma once

#include <wrl/client.h>

#include "d3dx12.h"

namespace Pixel {
	//TODO:need to inherit from vertex buffer
	class DirectXVertexBuffer
	{
	public:
		DirectXVertexBuffer(float* vertices, uint32_t ByteSize);
	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_vertexBuffer;
	};
}
