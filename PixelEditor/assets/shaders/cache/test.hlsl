//------mesh constants------
cbuffer cbPerObject : register(b0)
{
	float4x4 gWorld;//world matrix
	float4x4 ginvWorld;//inverse world matrix
	float4x4 previousWorld;//use for TAA
	int gEditor;
};
//------mesh constants------

cbuffer cbPass : register(b1)
{
	float4x4 gViewProjection;
	float4x4 gPreviousViewProjection;//use for TAA
	uint gFrameCount;
	float gWidth;
	float gHeight;
};

cbuffer CbMaterial : register(b2)
{
	float3 gAlbedo;
	float gRoughness;
	float gMetallic;
	float gAo;
	bool HaveNormal;//have normal
	int ShadingModelID;//shading model id
	float ClearCoat;//for clear coat
	float ClearCoatRoughness;//for clear coat
};

struct VertexIn
{
	float3 PosL : POSITION;
	float2 TexCoord : TEXCOORD;
	float3 NormalL : NORMAL;
	//int Editor : EDITOR;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float2 TexCoord : TEXCOORD;
	float3 NormalW : NORMAL;
	int Editor : EDITOR;

	//------use for TAA------
	float4 preScreenPosition : POSITION2;
	float4 nowScreenPosition : POSITION3;
	//------use for TAA------
};

struct PixelOut
{
	float4 gBufferPosition : SV_Target;
	float4 gBufferNormal : SV_Target1;
	float4 gVelocity : SV_Target2;
	float4 gBufferAlbedo : SV_Target3;
	float4 gBufferRoughnessMetallicAo : SV_Target4;
	int gEditor : SV_Target5;
};

static const float2 Halton_2_3[8] =
{
	float2(0.0f, -1.0f / 3.0f),
	float2(-1.0f / 2.0f, 1.0f / 3.0f),
	float2(1.0f / 2.0f, -7.0f / 9.0f),
	float2(-3.0f / 4.0f, -1.0f / 9.0f),
	float2(1.0f / 4.0f, 5.0f / 9.0f),
	float2(-1.0f / 4.0f, -5.0f / 9.0f),
	float2(3.0f / 4.0f, 1.0f / 9.0f),
	float2(-7.0f / 8.0f, 7.0f / 9.0f)
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout = (VertexOut)0.0f;

	//------calculate jitter------
	float deltaWidth = 1.0f / gWidth;
	float deltaHeight = 1.0f / gHeight;

	uint index = gFrameCount % 8;
	float2 jitter = float2(Halton_2_3[index].x * deltaWidth, Halton_2_3[index].y * deltaHeight);
	float4x4 jitterMat = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		jitter.x, jitter.y, 0.0f, 1.0f
	};
	vout.preScreenPosition = mul(mul(float4(vin.PosL, 1.0f), previousWorld), gPreviousViewProjection);
	vout.nowScreenPosition = mul(mul(float4(vin.PosL, 1.0f), gWorld), gViewProjection);
	//------calculate jitter------

	//to world space
	float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
	vout.PosW = posW.xyz;

	vout.NormalW = mul(vin.NormalL, (float3x3)transpose(ginvWorld));

	//homogeneous clipping
	//vout.PosH = mul(posW, gView);
	vout.PosH = mul(mul(posW, gViewProjection), jitterMat);

	vout.TexCoord = vin.TexCoord;

	vout.Editor = gEditor;
	return vout;
}

//------material texture------
Texture2D gAlbedoMap : register(t0);
Texture2D gNormalMap : register(t1);
Texture2D gRoughnessMap : register(t2);
Texture2D gMetallicMap : register(t3);
Texture2D gAoMap : register(t4);
//------material texture------

//------material samplers------
SamplerState gsamPointWrap : register(s0);//static sampler
//------material samplers------
#include "../Common/Common.hlsl"
PixelOut PS(VertexOut pin){
float4  ConstFloatValue7;
ConstFloatValue7 = float4(1.000000, 0.999990, 0.999990, 0.000000);
float4  ConstFloatValue15;
ConstFloatValue15 = float4(0.762548, 1.000000, 0.050193, 0.000000);
float4  MulInputA23 = ConstFloatValue7;
float4  MulInputB24 = ConstFloatValue15;
float4  MulOutput24 = float4(0, 0, 0, 1);
MulOutput24 = MulInputA23 * MulInputB24;
float4  Normal = float4(0, 0, 0, 1);
float4  Albedo = MulOutput24;
float  Roughness = 0;
float  Metallic = 0;
float  Ao = 0;
PixelOut pixelOut = (PixelOut)(0.0f);
pixelOut.gBufferPosition.xyz = pin.PosW;
if (HaveNormal)
pixelOut.gBufferNormal.xyz = (pin.NormalW.xyz + 1.0f) / 2.0f;
else
pixelOut.gBufferNormal.xyz = (DecodeNormalMap(pin.TexCoord, pin.PosW, pin.NormalW) + 1.0f) / 2.0f;
pixelOut.gBufferAlbedo.xyz = Albedo.xyz;
pixelOut.gBufferAlbedo.w = ClearCoatRoughness;
pixelOut.gVelocity.w = ClearCoat;
pixelOut.gBufferRoughnessMetallicAo.x = Roughness;
pixelOut.gBufferRoughnessMetallicAo.y = Metallic;
pixelOut.gBufferRoughnessMetallicAo.z = Ao;
pixelOut.gBufferRoughnessMetallicAo.w = ShadingModelID / 255.0f;
float2 newPos = ((pin.nowScreenPosition.xy / pin.nowScreenPosition.w) * 0.5f + 0.5f);
float2 prePos = ((pin.preScreenPosition.xy / pin.preScreenPosition.w) * 0.5f + 0.5f);
pixelOut.gVelocity.xy = newPos - prePos;
pixelOut.gVelocity.z = 0.0f;
return pixelOut;
}