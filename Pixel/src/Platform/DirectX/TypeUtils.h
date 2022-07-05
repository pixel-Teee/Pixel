#pragma once

#include "Pixel/Renderer/RendererType.h"
#include "Pixel/Renderer/Descriptor/DescriptorHeap.h"
#include "Pixel/Renderer/PipelineStateObject/RootParameter.h"
#include "Platform/DirectX/d3dx12.h"
#include "Pixel/Renderer/Context/ContextType.h"

namespace Pixel {

	D3D12_DESCRIPTOR_HEAP_TYPE DescriptorHeapTypeToDirectXDescriptorHeapType(DescriptorHeapType Type);

	DescriptorHeapType DirectXDescriptorHeapTypeToDescriptorHeapType(D3D12_DESCRIPTOR_HEAP_TYPE Type);

	D3D12_SHADER_VISIBILITY ShaderVisibilityToDirectXShaderVisibility(ShaderVisibility Visibility);

	D3D12_DESCRIPTOR_RANGE_TYPE RangeTypeToDirectXRangeType(RangeType Type);

	DXGI_FORMAT ImageFormatToDirectXImageFormat(ImageFormat Format);

	D3D12_RECT RectToDirectXRect(PixelRect Rect);

	D3D12_VIEWPORT ViewPortToDirectXViewPort(ViewPort viewPort);

	D3D12_PRIMITIVE_TOPOLOGY PrimitiveTopologyToDirectXPrimitiveTopology(PrimitiveTopology Topology);

	D3D12_PRIMITIVE_TOPOLOGY_TYPE PiplinePrimitiveTopologyToDirectXPrimitiveTopology(PiplinePrimitiveTopology Topology);

	D3D12_RESOURCE_STATES ResourceStatesToDirectXResourceStates(ResourceStates States);

	D3D12_COMMAND_LIST_TYPE CmdListTypeToDirectXCmdListType(CommandListType CmdListType);

	CommandListType DirectXCmdListTypeToCmdListType(D3D12_COMMAND_LIST_TYPE CmdListType);

	D3D12_ROOT_SIGNATURE_FLAGS RootSignatureFlagToDirectXRootSignatureFlag(RootSignatureFlag flag);

	D3D12_TEXTURE_ADDRESS_MODE AddressModeToDirectXAddressMode(AddressMode addressMode);

	D3D12_FILTER FilterToDirectXFilter(Filter filter);

	uint32_t CalculateSubresource(uint32_t MipSlice, uint32_t ArraySlice, uint32_t MipLevels, uint32_t ArraySize, uint32_t PlaneSlice);
}
