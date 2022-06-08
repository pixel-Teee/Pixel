#pragma once

#include "Pixel/Renderer/Descriptor/DescriptorHeap.h"
#include "Pixel/Renderer/PipelineStateObject/RootParameter.h"
#include "Pixel/Renderer/Buffer/PixelBuffer.h"
#include "Platform/DirectX/d3dx12.h"

namespace Pixel {

	D3D12_DESCRIPTOR_HEAP_TYPE DescriptorHeapTypeToDirectXDescriptorHeapType(DescriptorHeapType Type);

	D3D12_SHADER_VISIBILITY ShaderVisibilityToDirectXShaderVisibility(ShaderVisibility Visibility);

	D3D12_DESCRIPTOR_RANGE_TYPE RangeTypeToDirectXRangeType(RangeType Type);

	DXGI_FORMAT ImageFormatToDirectXImageFormat(ImageFormat Format);
}
