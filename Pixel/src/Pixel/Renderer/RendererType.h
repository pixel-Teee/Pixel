#pragma once

#include <glm/glm.hpp>

namespace Pixel {

	struct PixelRect {
		int32_t Left;
		int32_t Right;
		int32_t Top;
		int32_t Bottom;
	};

	struct ViewPort
	{
		float TopLeftX;
		float TopLeftY;
		float Width;
		float Height;
		float MinDepth;
		float MaxDepth;
	};

	enum class PiplinePrimitiveTopology
	{
		UNDEFINED,
		POINT,
		LINE,
		TRIANGLE,
		PATCH
	};

	enum class PrimitiveTopology
	{
		UNDEFINED,
		LINELIST,
		LINESTRIP,
		TRIANGLELIST,
		TRIANGLESTRIP
	};

	enum class ResourceStates
	{
		Common,
		VertexAndConstantBuffer,
		IndexBuffer,
		RenderTarget,
		UnorderedAccess,
		DepthWrite,
		DepthRead,
		NonPixelShaderResource,
		PixelShaderResource,
		StreamOut,
		IndirectArgument,
		CopyDest,
		CopySource,
		ResolveDest,
		ResolveSource,
		RaytracingAccelerationStructure,
		ShadingRateSource,
		GenericRead,
		Present
	};

	enum class RootSignatureFlag {
		AllowInputAssemblerInputLayout
	};

	enum class ShaderVisibility
	{
		ALL = 0,
		Vertex,
		Hull,
		Domain,
		Geometry,
		Pixel,
		Amplification,
		Mesh
	};

	enum class BlenderStateType
	{
		BlendDisable
	};

	enum class ImageFormat {
		PX_FORMAT_UNKNOWN = 0,
		PX_FORMAT_R32G32B32A32_TYPELESS = 1,
		PX_FORMAT_R32G32B32A32_FLOAT = 2,
		PX_FORMAT_R32G32B32A32_UINT = 3,
		PX_FORMAT_R32G32B32A32_SINT = 4,
		PX_FORMAT_R32G32B32_TYPELESS = 5,
		PX_FORMAT_R32G32B32_FLOAT = 6,
		PX_FORMAT_R32G32B32_UINT = 7,
		PX_FORMAT_R32G32B32_SINT = 8,
		PX_FORMAT_R16G16B16A16_TYPELESS = 9,
		PX_FORMAT_R16G16B16A16_FLOAT = 10,
		PX_FORMAT_R16G16B16A16_UNORM = 11,
		PX_FORMAT_R16G16B16A16_UINT = 12,
		PX_FORMAT_R16G16B16A16_SNORM = 13,
		PX_FORMAT_R16G16B16A16_SINT = 14,
		PX_FORMAT_R32G32_TYPELESS = 15,
		PX_FORMAT_R32G32_FLOAT = 16,
		PX_FORMAT_R32G32_UINT = 17,
		PX_FORMAT_R32G32_SINT = 18,
		PX_FORMAT_R32G8X24_TYPELESS = 19,
		PX_FORMAT_D32_FLOAT_S8X24_UINT = 20,
		PX_FORMAT_R32_FLOAT_X8X24_TYPELESS = 21,
		PX_FORMAT_X32_TYPELESS_G8X24_UINT = 22,
		PX_FORMAT_R10G10B10A2_TYPELESS = 23,
		PX_FORMAT_R10G10B10A2_UNORM = 24,
		PX_FORMAT_R10G10B10A2_UINT = 25,
		PX_FORMAT_R11G11B10_FLOAT = 26,
		PX_FORMAT_R8G8B8A8_TYPELESS = 27,
		PX_FORMAT_R8G8B8A8_UNORM = 28,
		PX_FORMAT_R8G8B8A8_UNORM_SRGB = 29,
		PX_FORMAT_R8G8B8A8_UINT = 30,
		PX_FORMAT_R8G8B8A8_SNORM = 31,
		PX_FORMAT_R8G8B8A8_SINT = 32,
		PX_FORMAT_R16G16_TYPELESS = 33,
		PX_FORMAT_R16G16_FLOAT = 34,
		PX_FORMAT_R16G16_UNORM = 35,
		PX_FORMAT_R16G16_UINT = 36,
		PX_FORMAT_R16G16_SNORM = 37,
		PX_FORMAT_R16G16_SINT = 38,
		PX_FORMAT_R32_TYPELESS = 39,
		PX_FORMAT_D32_FLOAT = 40,
		PX_FORMAT_R32_FLOAT = 41,
		PX_FORMAT_R32_UINT = 42,
		PX_FORMAT_R32_SINT = 43,
		PX_FORMAT_R24G8_TYPELESS = 44,
		PX_FORMAT_D24_UNORM_S8_UINT = 45,
		PX_FORMAT_R24_UNORM_X8_TYPELESS = 46,
		PX_FORMAT_X24_TYPELESS_G8_UINT = 47,
		PX_FORMAT_R8G8_TYPELESS = 48,
		PX_FORMAT_R8G8_UNORM = 49,
		PX_FORMAT_R8G8_UINT = 50,
		PX_FORMAT_R8G8_SNORM = 51,
		PX_FORMAT_R8G8_SINT = 52,
		PX_FORMAT_R16_TYPELESS = 53,
		PX_FORMAT_R16_FLOAT = 54,
		PX_FORMAT_D16_UNORM = 55,
		PX_FORMAT_R16_UNORM = 56,
		PX_FORMAT_R16_UINT = 57,
		PX_FORMAT_R16_SNORM = 58,
		PX_FORMAT_R16_SINT = 59,
		PX_FORMAT_R8_TYPELESS = 60,
		PX_FORMAT_R8_UNORM = 61,
		PX_FORMAT_R8_UINT = 62,
		PX_FORMAT_R8_SNORM = 63,
		PX_FORMAT_R8_SINT = 64,
		PX_FORMAT_A8_UNORM = 65,
		PX_FORMAT_R1_UNORM = 66,
		PX_FORMAT_R9G9B9E5_SHAREDEXP = 67,
		PX_FORMAT_R8G8_B8G8_UNORM = 68,
		PX_FORMAT_G8R8_G8B8_UNORM = 69,
		PX_FORMAT_BC1_TYPELESS = 70,
		PX_FORMAT_BC1_UNORM = 71,
		PX_FORMAT_BC1_UNORM_SRGB = 72,
		PX_FORMAT_BC2_TYPELESS = 73,
		PX_FORMAT_BC2_UNORM = 74,
		PX_FORMAT_BC2_UNORM_SRGB = 75,
		PX_FORMAT_BC3_TYPELESS = 76,
		PX_FORMAT_BC3_UNORM = 77,
		PX_FORMAT_BC3_UNORM_SRGB = 78,
		PX_FORMAT_BC4_TYPELESS = 79,
		PX_FORMAT_BC4_UNORM = 80,
		PX_FORMAT_BC4_SNORM = 81,
		PX_FORMAT_BC5_TYPELESS = 82,
		PX_FORMAT_BC5_UNORM = 83,
		PX_FORMAT_BC5_SNORM = 84,
		PX_FORMAT_B5G6R5_UNORM = 85,
		PX_FORMAT_B5G5R5A1_UNORM = 86,
		PX_FORMAT_B8G8R8A8_UNORM = 87,
		PX_FORMAT_B8G8R8X8_UNORM = 88,
		PX_FORMAT_R10G10B10_XR_BIAS_A2_UNORM = 89,
		PX_FORMAT_B8G8R8A8_TYPELESS = 90,
		PX_FORMAT_B8G8R8A8_UNORM_SRGB = 91,
		PX_FORMAT_B8G8R8X8_TYPELESS = 92,
		PX_FORMAT_B8G8R8X8_UNORM_SRGB = 93,
		PX_FORMAT_BC6H_TYPELESS = 94,
		PX_FORMAT_BC6H_UF16 = 95,
		PX_FORMAT_BC6H_SF16 = 96,
		PX_FORMAT_BC7_TYPELESS = 97,
		PX_FORMAT_BC7_UNORM = 98,
		PX_FORMAT_BC7_UNORM_SRGB = 99,
		PX_FORMAT_AYUV = 100,
		PX_FORMAT_Y410 = 101,
		PX_FORMAT_Y416 = 102,
		PX_FORMAT_NV12 = 103,
		PX_FORMAT_P010 = 104,
		PX_FORMAT_P016 = 105,
		PX_FORMAT_420_OPAQUE = 106,
		PX_FORMAT_YUY2 = 107,
		PX_FORMAT_Y210 = 108,
		PX_FORMAT_Y216 = 109,
		PX_FORMAT_NV11 = 110,
		PX_FORMAT_AI44 = 111,
		PX_FORMAT_IA44 = 112,
		PX_FORMAT_P8 = 113,
		PX_FORMAT_A8P8 = 114,
		PX_FORMAT_B4G4R4A4_UNORM = 115,
		PX_FORMAT_P208 = 130,
		PX_FORMAT_V208 = 131,
		PX_FORMAT_V408 = 132,
		PX_FORMAT_SAMPLER_FEEDBACK_MIN_MIP_OPAQUE = 189,
		PX_FORMAT_SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE = 190,
		PX_FORMAT_FORCE_UINT = 0xfffffff
	};

	enum class RangeType
	{
		SRV = 0,
		UAV,
		CBV,
		SAMPLER
	};

	enum class RootBindings
	{
		MeshConstants,
		MaterialConstants,
		MaterialSRVs,
		MaterialSamplers,
		CommonSRVs,
		CommonCBV,
		SkinMatrices,
		NumRootBindings
	};

#define MAXLIGHTS 16

	struct Light
	{
		glm::vec3 Position;//point light, to calculate attenuation
		float pad;
		glm::vec3 Direction;//direction light
		float pad2;
		glm::vec3 Color;//point light/direction light
		float Radius;//point light, volume radius
		uint32_t GenerateShadow = 0;//cast shadow
		glm::vec3 pad3;
	};

	struct alignas(256) LightPass
	{
		glm::vec3 CameraPosition;
		int PointLightNumber = 0;
		int DirectLightNumber = 0;
		int SpotLightNumber = 0;
		float pad;
		float pad2;
		glm::mat4 LightSpaceMatrix;
		int receiveAmbientLight;
		float pad3;
		float pad4;
		float pad5;
		Light lights[MAXLIGHTS];
	};

	struct alignas(256) MaterialConstant
	{
		glm::vec3 Albedo = { 1.0f, 1.0f, 1.0f };
		float Roughness = 1.0f;
		float Metallic = 0.0f;
		float Emissive = 0.0f;
		uint32_t HaveNormal = false;
		uint32_t shadingModel = 0;
		float ClearCoat = 1.0f;
		float ClearCoatRoughness = 1.0f;
	};

	enum class AddressMode
	{
		CLAMP,
		WRAP,
		BORDER
	};

	enum class Filter
	{
		MIN_MAG_MIP_LINEAR
	};

	//------use for geometry pass------
	struct alignas(256) CbufferGeometryPass
	{
		glm::mat4 ViewProjection;
		glm::mat4 previousViewProjection;//use for TAA
		uint32_t frameCount;
		float width;
		float height;
	};
	//------use for geometry pass------
}