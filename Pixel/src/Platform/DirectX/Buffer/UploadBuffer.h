#pragma once

#include "DirectXGpuResource.h"

namespace Pixel {
	class Device;
	class UploadBuffer : public DirectXGpuResource
	{
	public:
		virtual ~UploadBuffer() { Destroy(); }

		void Create(const std::wstring& name, size_t BufferSize, Ref<Device> pDevice);

		void* Map();
		void UnMap(size_t begin, size_t end = -1);

		size_t GetBufferSize() const { return m_BufferSize; }

	protected:
		size_t m_BufferSize;
	};
}