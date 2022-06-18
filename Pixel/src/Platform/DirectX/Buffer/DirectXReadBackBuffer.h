#pragma once

#include "Platform/DirectX/Buffer/DirectXGpuBuffer.h"

namespace Pixel {

	class ReadBackBuffer : public DirectXGpuBuffer
	{
	public:
		virtual ~ReadBackBuffer() { Destroy(); }

		void Create(const std::wstring& name, uint32_t NumElements,
			uint32_t ElementSize);

		void* Map();
		void UnMap();
	};
}
