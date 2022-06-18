#pragma once

#include "Platform/DirectX/Buffer/DirectXGpuBuffer.h"

namespace Pixel {
	class StructuredBuffer : public DirectXGpuBuffer
	{
	public:
		virtual ~StructuredBuffer();
		virtual void CreateDerivedViews() override;
	};
}
