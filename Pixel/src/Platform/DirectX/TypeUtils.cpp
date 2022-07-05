#include "pxpch.h"

#include "TypeUtils.h"

namespace Pixel {
	D3D12_DESCRIPTOR_HEAP_TYPE DescriptorHeapTypeToDirectXDescriptorHeapType(DescriptorHeapType Type)
	{
		switch (Type)
		{
		case DescriptorHeapType::CBV_UAV_SRV:
			return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		case DescriptorHeapType::SAMPLER:
			return D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
		case DescriptorHeapType::RTV:
			return D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		case DescriptorHeapType::DSV:
			return D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		}
	}

	DescriptorHeapType DirectXDescriptorHeapTypeToDescriptorHeapType(D3D12_DESCRIPTOR_HEAP_TYPE Type)
	{
		switch (Type)
		{
		case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
			return DescriptorHeapType::CBV_UAV_SRV;
		case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
			return DescriptorHeapType::SAMPLER;
		case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
			return DescriptorHeapType::RTV;
		case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
			return DescriptorHeapType::DSV;
		}
	}

	D3D12_SHADER_VISIBILITY ShaderVisibilityToDirectXShaderVisibility(ShaderVisibility Visibility)
	{
		switch (Visibility)
		{
		case ShaderVisibility::ALL:
			return D3D12_SHADER_VISIBILITY_ALL;
		case ShaderVisibility::Vertex:
			return D3D12_SHADER_VISIBILITY_VERTEX;
		case ShaderVisibility::Hull:
			return D3D12_SHADER_VISIBILITY_HULL;
		case ShaderVisibility::Domain:
			return D3D12_SHADER_VISIBILITY_DOMAIN;
		case ShaderVisibility::Geometry:
			return D3D12_SHADER_VISIBILITY_GEOMETRY;
		case ShaderVisibility::Pixel:
			return D3D12_SHADER_VISIBILITY_PIXEL;
		case ShaderVisibility::Amplification:
			return D3D12_SHADER_VISIBILITY_AMPLIFICATION;
		case Pixel::ShaderVisibility::Mesh:
			return D3D12_SHADER_VISIBILITY_MESH;
		}
	}

	D3D12_DESCRIPTOR_RANGE_TYPE RangeTypeToDirectXRangeType(RangeType Type)
	{
		switch (Type)
		{
		case Pixel::RangeType::SRV:
			return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		case Pixel::RangeType::UAV:
			return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
		case Pixel::RangeType::CBV:
			return D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		case Pixel::RangeType::SAMPLER:
			return D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
		}
	}

	DXGI_FORMAT ImageFormatToDirectXImageFormat(ImageFormat Format)
	{
		switch (Format)
		{
		case ImageFormat::PX_FORMAT_UNKNOWN:
			return DXGI_FORMAT_UNKNOWN;
		case ImageFormat::PX_FORMAT_R32G32B32A32_TYPELESS:
			return DXGI_FORMAT_R32G32B32A32_TYPELESS;
		case ImageFormat::PX_FORMAT_R32G32B32A32_FLOAT:
			return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case ImageFormat::PX_FORMAT_R32G32B32A32_UINT:
			return DXGI_FORMAT_R32G32B32A32_UINT;
		case ImageFormat::PX_FORMAT_R32G32B32A32_SINT:
			return DXGI_FORMAT_R32G32B32A32_SINT;
		case ImageFormat::PX_FORMAT_R32G32B32_TYPELESS:
			return DXGI_FORMAT_R32G32B32_TYPELESS;
		case ImageFormat::PX_FORMAT_R32G32B32_FLOAT:
			return DXGI_FORMAT_R32G32B32_FLOAT;
		case ImageFormat::PX_FORMAT_R32G32B32_UINT:
			return DXGI_FORMAT_R32G32B32_UINT;
		case ImageFormat::PX_FORMAT_R32G32B32_SINT:
			return DXGI_FORMAT_R32G32B32_SINT;
		case ImageFormat::PX_FORMAT_R16G16B16A16_TYPELESS:
			return DXGI_FORMAT_R16G16B16A16_TYPELESS;
		case ImageFormat::PX_FORMAT_R16G16B16A16_FLOAT:
			return DXGI_FORMAT_R16G16B16A16_FLOAT;
		case ImageFormat::PX_FORMAT_R16G16B16A16_UNORM:
			return DXGI_FORMAT_R16G16B16A16_UNORM;
		case ImageFormat::PX_FORMAT_R16G16B16A16_UINT:
			return DXGI_FORMAT_R16G16B16A16_UINT;
		case ImageFormat::PX_FORMAT_R16G16B16A16_SNORM:
			return DXGI_FORMAT_R16G16B16A16_SNORM;
		case ImageFormat::PX_FORMAT_R16G16B16A16_SINT:
			return DXGI_FORMAT_R16G16B16A16_SINT;
		case ImageFormat::PX_FORMAT_R32G32_TYPELESS:
			return DXGI_FORMAT_R32G32_TYPELESS;
		case ImageFormat::PX_FORMAT_R32G32_FLOAT:
			return DXGI_FORMAT_R32G32_FLOAT;
		case ImageFormat::PX_FORMAT_R32G32_UINT:
			return DXGI_FORMAT_R32G32_UINT;
		case ImageFormat::PX_FORMAT_R32G32_SINT://
			return DXGI_FORMAT_R32G32_SINT;
		case ImageFormat::PX_FORMAT_R32G8X24_TYPELESS:
			return DXGI_FORMAT_R32G8X24_TYPELESS;
		case ImageFormat::PX_FORMAT_D32_FLOAT_S8X24_UINT:
			return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
		case ImageFormat::PX_FORMAT_R32_FLOAT_X8X24_TYPELESS:
			return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
		case ImageFormat::PX_FORMAT_X32_TYPELESS_G8X24_UINT:
			return DXGI_FORMAT_X32_TYPELESS_G8X24_UINT;
		case ImageFormat::PX_FORMAT_R10G10B10A2_TYPELESS:
			return DXGI_FORMAT_R10G10B10A2_TYPELESS;
		case ImageFormat::PX_FORMAT_R10G10B10A2_UNORM:
			return DXGI_FORMAT_R10G10B10A2_UNORM;
		case ImageFormat::PX_FORMAT_R10G10B10A2_UINT:
			return DXGI_FORMAT_R10G10B10A2_UINT;
		case ImageFormat::PX_FORMAT_R11G11B10_FLOAT:
			return DXGI_FORMAT_R11G11B10_FLOAT;//
		case ImageFormat::PX_FORMAT_R8G8B8A8_TYPELESS:
			return DXGI_FORMAT_R8G8B8A8_TYPELESS;
		case ImageFormat::PX_FORMAT_R8G8B8A8_UNORM:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
		case ImageFormat::PX_FORMAT_R8G8B8A8_UNORM_SRGB:
			return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		case ImageFormat::PX_FORMAT_R8G8B8A8_UINT:
			return DXGI_FORMAT_R8G8B8A8_UINT;
		case ImageFormat::PX_FORMAT_R8G8B8A8_SNORM:
			return DXGI_FORMAT_R8G8B8A8_SNORM;
		case ImageFormat::PX_FORMAT_R8G8B8A8_SINT:
			return DXGI_FORMAT_R8G8B8A8_SINT;
		case ImageFormat::PX_FORMAT_R16G16_TYPELESS:
			return DXGI_FORMAT_R16G16_TYPELESS;
		case ImageFormat::PX_FORMAT_R16G16_FLOAT:
			return DXGI_FORMAT_R16G16_FLOAT;
		case ImageFormat::PX_FORMAT_R16G16_UNORM:
			return DXGI_FORMAT_R16G16_UNORM;
		case ImageFormat::PX_FORMAT_R16G16_UINT:
			return DXGI_FORMAT_R16G16_UINT;//
		case ImageFormat::PX_FORMAT_R16G16_SNORM:
			return DXGI_FORMAT_R16G16_SNORM;
		case ImageFormat::PX_FORMAT_R16G16_SINT:
			return DXGI_FORMAT_R16G16_SINT;
		case ImageFormat::PX_FORMAT_R32_TYPELESS:
			return DXGI_FORMAT_R32_TYPELESS;
		case ImageFormat::PX_FORMAT_D32_FLOAT:
			return DXGI_FORMAT_D32_FLOAT;
		case ImageFormat::PX_FORMAT_R32_FLOAT:
			return DXGI_FORMAT_R32_FLOAT;
		case ImageFormat::PX_FORMAT_R32_UINT:
			return DXGI_FORMAT_R32_UINT;
		case ImageFormat::PX_FORMAT_R32_SINT:
			return DXGI_FORMAT_R32_SINT;
		case ImageFormat::PX_FORMAT_R24G8_TYPELESS:
			return DXGI_FORMAT_R24G8_TYPELESS;
		case ImageFormat::PX_FORMAT_D24_UNORM_S8_UINT:
			return DXGI_FORMAT_D24_UNORM_S8_UINT;
		case ImageFormat::PX_FORMAT_R24_UNORM_X8_TYPELESS:
			return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		case ImageFormat::PX_FORMAT_X24_TYPELESS_G8_UINT:
			return DXGI_FORMAT_X24_TYPELESS_G8_UINT;
		case ImageFormat::PX_FORMAT_R8G8_TYPELESS:
			return DXGI_FORMAT_R8G8_TYPELESS;
		case ImageFormat::PX_FORMAT_R8G8_UNORM:
			return DXGI_FORMAT_R8G8_UNORM;
		case ImageFormat::PX_FORMAT_R8G8_UINT:
			return DXGI_FORMAT_R8G8_UINT;
		case ImageFormat::PX_FORMAT_R8G8_SNORM:
			return DXGI_FORMAT_R8G8_SNORM;
		case ImageFormat::PX_FORMAT_R8G8_SINT:
			return DXGI_FORMAT_R8G8_SINT;
		case ImageFormat::PX_FORMAT_R16_TYPELESS:
			return DXGI_FORMAT_R16_TYPELESS;
		case ImageFormat::PX_FORMAT_R16_FLOAT:
			return DXGI_FORMAT_R16_FLOAT;
		case ImageFormat::PX_FORMAT_D16_UNORM:
			return DXGI_FORMAT_D16_UNORM;
		case ImageFormat::PX_FORMAT_R16_UNORM:
			return DXGI_FORMAT_R16_UNORM;
		case ImageFormat::PX_FORMAT_R16_UINT:
			return DXGI_FORMAT_R16_UINT;
		case ImageFormat::PX_FORMAT_R16_SNORM:
			return DXGI_FORMAT_R16_SNORM;
		case ImageFormat::PX_FORMAT_R16_SINT:
			return DXGI_FORMAT_R16_SINT;
		case ImageFormat::PX_FORMAT_R8_TYPELESS:
			return DXGI_FORMAT_R8_TYPELESS;
		case ImageFormat::PX_FORMAT_R8_UNORM:
			return DXGI_FORMAT_R8_UNORM;
		case ImageFormat::PX_FORMAT_R8_UINT:
			return DXGI_FORMAT_R8_UINT;
		case ImageFormat::PX_FORMAT_R8_SNORM:
			return DXGI_FORMAT_R8_SNORM;
		case ImageFormat::PX_FORMAT_R8_SINT:
			return DXGI_FORMAT_R8_SINT;
		case ImageFormat::PX_FORMAT_A8_UNORM:
			return DXGI_FORMAT_A8_UNORM;
		case ImageFormat::PX_FORMAT_R1_UNORM:
			return DXGI_FORMAT_R1_UNORM;
		case ImageFormat::PX_FORMAT_R9G9B9E5_SHAREDEXP:
			return DXGI_FORMAT_R9G9B9E5_SHAREDEXP;
		case ImageFormat::PX_FORMAT_R8G8_B8G8_UNORM:
			return DXGI_FORMAT_R8G8_B8G8_UNORM;
		case ImageFormat::PX_FORMAT_G8R8_G8B8_UNORM:
			return DXGI_FORMAT_G8R8_G8B8_UNORM;
		case ImageFormat::PX_FORMAT_BC1_TYPELESS:
			return DXGI_FORMAT_BC1_TYPELESS;
		case ImageFormat::PX_FORMAT_BC1_UNORM:
			return DXGI_FORMAT_BC1_UNORM;
		case ImageFormat::PX_FORMAT_BC1_UNORM_SRGB:
			return DXGI_FORMAT_BC1_UNORM_SRGB;//
		case ImageFormat::PX_FORMAT_BC2_TYPELESS:
			return DXGI_FORMAT_BC2_TYPELESS;
		case ImageFormat::PX_FORMAT_BC2_UNORM:
			return DXGI_FORMAT_BC2_UNORM;
		case ImageFormat::PX_FORMAT_BC2_UNORM_SRGB:
			return DXGI_FORMAT_BC2_UNORM_SRGB;
		case ImageFormat::PX_FORMAT_BC3_TYPELESS:
			return DXGI_FORMAT_BC3_TYPELESS;
		case ImageFormat::PX_FORMAT_BC3_UNORM:
			return DXGI_FORMAT_BC3_UNORM;
		case ImageFormat::PX_FORMAT_BC3_UNORM_SRGB:
			return DXGI_FORMAT_BC3_UNORM_SRGB;
		case ImageFormat::PX_FORMAT_BC4_TYPELESS:
			return DXGI_FORMAT_BC4_TYPELESS;
		case ImageFormat::PX_FORMAT_BC4_UNORM:
			return DXGI_FORMAT_BC4_UNORM;
		case ImageFormat::PX_FORMAT_BC4_SNORM:
			return DXGI_FORMAT_BC4_SNORM;
		case ImageFormat::PX_FORMAT_BC5_TYPELESS:
			return DXGI_FORMAT_BC5_TYPELESS;
		case ImageFormat::PX_FORMAT_BC5_UNORM:
			return DXGI_FORMAT_BC5_UNORM;
		case ImageFormat::PX_FORMAT_BC5_SNORM:
			return DXGI_FORMAT_BC5_SNORM;
		case ImageFormat::PX_FORMAT_B5G6R5_UNORM:
			return DXGI_FORMAT_B5G6R5_UNORM;
		case ImageFormat::PX_FORMAT_B5G5R5A1_UNORM:
			return DXGI_FORMAT_B5G5R5A1_UNORM;
		case ImageFormat::PX_FORMAT_B8G8R8A8_UNORM:
			return DXGI_FORMAT_B8G8R8A8_UNORM;
		case ImageFormat::PX_FORMAT_B8G8R8X8_UNORM:
			return DXGI_FORMAT_B8G8R8X8_UNORM;
		case ImageFormat::PX_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
			return DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM;
		case ImageFormat::PX_FORMAT_B8G8R8A8_TYPELESS:
			return DXGI_FORMAT_B8G8R8A8_TYPELESS;
		case ImageFormat::PX_FORMAT_B8G8R8A8_UNORM_SRGB:
			return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
		case ImageFormat::PX_FORMAT_B8G8R8X8_TYPELESS:
			return DXGI_FORMAT_B8G8R8X8_TYPELESS;
		case ImageFormat::PX_FORMAT_B8G8R8X8_UNORM_SRGB:
			return DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
		case ImageFormat::PX_FORMAT_BC6H_TYPELESS:
			return DXGI_FORMAT_BC6H_TYPELESS;
		case ImageFormat::PX_FORMAT_BC6H_UF16:
			return DXGI_FORMAT_BC6H_UF16;
		case ImageFormat::PX_FORMAT_BC6H_SF16:
			return DXGI_FORMAT_BC6H_SF16;
		case ImageFormat::PX_FORMAT_BC7_TYPELESS:
			return DXGI_FORMAT_BC7_TYPELESS;
		case ImageFormat::PX_FORMAT_BC7_UNORM:
			return DXGI_FORMAT_BC7_UNORM;
		case ImageFormat::PX_FORMAT_BC7_UNORM_SRGB:
			return DXGI_FORMAT_BC7_UNORM_SRGB;
		case ImageFormat::PX_FORMAT_AYUV:
			return DXGI_FORMAT_AYUV;
		case ImageFormat::PX_FORMAT_Y410:
			return DXGI_FORMAT_Y410;
		case ImageFormat::PX_FORMAT_Y416:
			return DXGI_FORMAT_Y416;
		case ImageFormat::PX_FORMAT_NV12:
			return DXGI_FORMAT_NV12;
		case ImageFormat::PX_FORMAT_P010:
			return DXGI_FORMAT_P010;
		case ImageFormat::PX_FORMAT_P016:
			return DXGI_FORMAT_P016;
		case ImageFormat::PX_FORMAT_420_OPAQUE:
			return DXGI_FORMAT_420_OPAQUE;
		case ImageFormat::PX_FORMAT_YUY2:
			return DXGI_FORMAT_YUY2;
		case ImageFormat::PX_FORMAT_Y210:
			return DXGI_FORMAT_Y210;
		case ImageFormat::PX_FORMAT_Y216:
			return DXGI_FORMAT_Y216;
		case ImageFormat::PX_FORMAT_NV11:
			return DXGI_FORMAT_NV11;
		case ImageFormat::PX_FORMAT_AI44:
			return DXGI_FORMAT_AI44;
		case ImageFormat::PX_FORMAT_IA44:
			return DXGI_FORMAT_IA44;
		case ImageFormat::PX_FORMAT_P8:
			return DXGI_FORMAT_P8;
		case ImageFormat::PX_FORMAT_A8P8:
			return DXGI_FORMAT_A8P8;
		case ImageFormat::PX_FORMAT_B4G4R4A4_UNORM:
			return DXGI_FORMAT_B4G4R4A4_UNORM;
		case ImageFormat::PX_FORMAT_P208:
			return DXGI_FORMAT_P208;
		case ImageFormat::PX_FORMAT_V208:
			return DXGI_FORMAT_V208;
		case ImageFormat::PX_FORMAT_V408:
			return DXGI_FORMAT_V408;
		case ImageFormat::PX_FORMAT_SAMPLER_FEEDBACK_MIN_MIP_OPAQUE:
			return DXGI_FORMAT_SAMPLER_FEEDBACK_MIN_MIP_OPAQUE;
		case ImageFormat::PX_FORMAT_SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE:
			return DXGI_FORMAT_SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE;
		case ImageFormat::PX_FORMAT_FORCE_UINT:
			return DXGI_FORMAT_FORCE_UINT;
		default:
			break;
		}
	}

	D3D12_RECT RectToDirectXRect(PixelRect Rect)
	{
		D3D12_RECT ReturnRect;
		ReturnRect.left = Rect.Left;
		ReturnRect.right = Rect.Right;
		ReturnRect.top = Rect.Top;
		ReturnRect.bottom = Rect.Bottom;
		return ReturnRect;
	}

	D3D12_VIEWPORT ViewPortToDirectXViewPort(ViewPort viewPort)
	{
		D3D12_VIEWPORT DxViewPort;
		DxViewPort.TopLeftX = viewPort.TopLeftX;
		DxViewPort.TopLeftY = viewPort.TopLeftY;
		DxViewPort.Width = viewPort.Width;
		DxViewPort.Height = viewPort.Height;
		DxViewPort.MaxDepth = viewPort.MaxDepth;
		DxViewPort.MinDepth = viewPort.MinDepth;

		return DxViewPort;
	}

	D3D12_PRIMITIVE_TOPOLOGY PrimitiveTopologyToDirectXPrimitiveTopology(PrimitiveTopology Topology)
	{
		switch (Topology)
		{
		case PrimitiveTopology::UNDEFINED:
			return D3D12_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_UNDEFINED;
		case PrimitiveTopology::LINELIST:
			return D3D12_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_LINELIST;
		case PrimitiveTopology::LINESTRIP:
			return D3D12_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP;
		case PrimitiveTopology::TRIANGLELIST:
			return D3D12_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		case PrimitiveTopology::TRIANGLESTRIP:
			return D3D12_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		}
	}

	D3D12_PRIMITIVE_TOPOLOGY_TYPE PiplinePrimitiveTopologyToDirectXPrimitiveTopology(PiplinePrimitiveTopology Topology)
	{
		switch (Topology)
		{
		case PiplinePrimitiveTopology::UNDEFINED:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
		case PiplinePrimitiveTopology::POINT:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
		case PiplinePrimitiveTopology::LINE:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		case PiplinePrimitiveTopology::TRIANGLE:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		case PiplinePrimitiveTopology::PATCH:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
		}
	}

	D3D12_RESOURCE_STATES ResourceStatesToDirectXResourceStates(ResourceStates States)
	{
		switch (States)
		{
		case ResourceStates::Common:
			return D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON;
		case ResourceStates::VertexAndConstantBuffer:
			return D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
		case ResourceStates::IndexBuffer:
			return D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_INDEX_BUFFER;
		case ResourceStates::RenderTarget:
			return D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RENDER_TARGET;
		case ResourceStates::UnorderedAccess:
			return D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		case ResourceStates::DepthWrite:
			return D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_DEPTH_WRITE;
		case ResourceStates::DepthRead:
			return D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_DEPTH_READ;
		case ResourceStates::NonPixelShaderResource:
			return D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
		case ResourceStates::PixelShaderResource:
			return D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		case ResourceStates::StreamOut:
			return D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_STREAM_OUT;
		case ResourceStates::IndirectArgument:
			return D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
		case ResourceStates::CopyDest:
			return D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST;
		case ResourceStates::CopySource:
			return D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_SOURCE;
		case ResourceStates::ResolveDest:
			return D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RESOLVE_DEST;
		case ResourceStates::ResolveSource:
			return D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
		case ResourceStates::RaytracingAccelerationStructure:
			return D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
		case ResourceStates::ShadingRateSource:
			return D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_SHADING_RATE_SOURCE;
		case ResourceStates::GenericRead:
			return D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ;
		case ResourceStates::Present:
			return D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT;
		}
	}

	D3D12_COMMAND_LIST_TYPE CmdListTypeToDirectXCmdListType(CommandListType CmdListType)
	{
		switch (CmdListType)
		{
		case CommandListType::Graphics:
			return D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT;
		case CommandListType::Compute:
			return D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_COMPUTE;
		case CommandListType::Copy:
			return D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_COPY;
		}
	}

	CommandListType DirectXCmdListTypeToCmdListType(D3D12_COMMAND_LIST_TYPE Type)
	{
		switch (Type)
		{
		case D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT:
			return CommandListType::Graphics;
		case D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_COMPUTE:
			return CommandListType::Compute;
		case D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_COPY:
			return CommandListType::Copy;
		}
	}

	D3D12_ROOT_SIGNATURE_FLAGS RootSignatureFlagToDirectXRootSignatureFlag(RootSignatureFlag flag)
	{
		switch (flag)
		{
		case RootSignatureFlag::AllowInputAssemblerInputLayout:
			return D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		}
	}

	D3D12_TEXTURE_ADDRESS_MODE AddressModeToDirectXAddressMode(AddressMode addressMode)
	{
		switch (addressMode)
		{
		case AddressMode::CLAMP:
			return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			break;
		case AddressMode::WRAP:
			return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			break;
		}
	}

	D3D12_FILTER FilterToDirectXFilter(Filter filter)
	{
		switch (filter)
		{
		case Filter::MIN_MAG_MIP_LINEAR:
			return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		}
	}

	uint32_t CalculateSubresource(uint32_t MipSlice, uint32_t ArraySlice, uint32_t MipLevels, uint32_t ArraySize, uint32_t PlaneSlice)
	{
		return MipSlice + ArraySlice * MipLevels + PlaneSlice * MipLevels * ArraySlice;
	}

}