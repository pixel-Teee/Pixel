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
	float gEmissive;
	bool HaveNormal;//have normal
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
	float4 gBufferRoughnessMetallicEmissive : SV_Target4;
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
Texture2D gEmissiveMap : register(t4);
//------material texture------

//------material samplers------
SamplerState gsamPointWrap : register(s0);//static sampler
//------material samplers------

float3 DecodeNormalMap(float2 uv, float3 worldPos, float3 normal)
{
	float3 TangentNormal = gNormalMap.Sample(gsamPointWrap, uv).xyz * 2.0f - 1.0f;

	float3 Q1 = ddx(worldPos);
	float3 Q2 = ddy(worldPos);
	float2 st1 = ddx(uv);
	float2 st2 = ddy(uv);

	float3 N = normalize(normal);
	float3 T = normalize(Q1 * st2.y - Q2 * st1.y);
	float3 B = -normalize(cross(N, T));
	float3x3 TBN = float3x3(T, B, N);

	//let the texture map's normal to world's normal
	return normalize(mul(TangentNormal, TBN));
}

PixelOut PS(VertexOut pin)
{
	//write out to gbuffer
	PixelOut pixelOut = (PixelOut)(0.0f);
	pixelOut.gBufferPosition.xyz = pin.PosW;

	//if don't have normal map, then use the vertex's normal
	if (HaveNormal)
		pixelOut.gBufferNormal.xyz = (pin.NormalW.xyz + 1.0f) / 2.0f;//[-1, 1]->[0, 1]
	else
		pixelOut.gBufferNormal.xyz = (DecodeNormalMap(pin.TexCoord, pin.PosW, pin.NormalW) + 1.0f) / 2.0f;//[-1, 1]->[0, 1]
	pixelOut.gBufferAlbedo.xyz = gAlbedoMap.Sample(gsamPointWrap, pin.TexCoord).xyz * gAlbedo;
	pixelOut.gBufferAlbedo.w = 1.0f;
	pixelOut.gBufferRoughnessMetallicEmissive.x = gRoughnessMap.Sample(gsamPointWrap, pin.TexCoord).x * gRoughness;
	pixelOut.gBufferRoughnessMetallicEmissive.y = gMetallicMap.Sample(gsamPointWrap, pin.TexCoord).x * gMetallic;
	pixelOut.gBufferRoughnessMetallicEmissive.z = gEmissiveMap.Sample(gsamPointWrap, pin.TexCoord).x * gEmissive;
	pixelOut.gEditor = pin.Editor;

	//------velocity------
	float2 newPos = ((pin.nowScreenPosition.xy / pin.nowScreenPosition.w) * 0.5f + 0.5f);
	float2 prePos = ((pin.preScreenPosition.xy / pin.preScreenPosition.w) * 0.5f + 0.5f);
	pixelOut.gVelocity.xy = newPos - prePos;
	pixelOut.gVelocity.z = 0.0f;
	//------velocity------

	return pixelOut;
}	