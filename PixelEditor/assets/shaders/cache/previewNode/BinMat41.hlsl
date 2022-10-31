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

struct VertexIn
{
	float3 PosL : POSITION;
	float2 TexCoord : TEXCOORD;
	/*
	float3 NormalL : NORMAL;
	float3 TangentL : TANGENT;
	float3 BinormalL : BINORMAL;
	*/
	//int Editor : EDITOR;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float2 TexCoord : TEXCOORD;
	/*
	float3 NormalW : NORMAL;
	float3 TangentW : TANGENT;
	float3 BinormalW : BINORMAL;
	int Editor : EDITOR;

	//------use for TAA------
	float4 preScreenPosition : POSITION2;
	float4 nowScreenPosition : POSITION3;
	//------use for TAA------
	*/
};

struct PixelOut
{
	float4 finalColor : SV_Target;
	/*
	float4 gBufferPosition : SV_Target;
	
	float4 gBufferNormal : SV_Target1;
	float4 gVelocity : SV_Target2;
	float4 gBufferAlbedo : SV_Target3;
	float4 gBufferRoughnessMetallicAo : SV_Target4;
	int gEditor : SV_Target5;
	*/
};

/*
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
*/

VertexOut VS(VertexIn vin)
{
	VertexOut vout = (VertexOut)0.0f;

	/*
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
	vout.TangentW = mul(vin.TangentL, (float3x3)transpose(ginvWorld));
	//homogeneous clipping
	//vout.PosH = mul(posW, gView);
	vout.PosH = mul(mul(posW, gViewProjection), jitterMat);

	vout.TexCoord = vin.TexCoord;

	vout.Editor = gEditor;
	*/

	vout.PosH = float4(vin.PosL, 1.0f);//draw quad
	vout.TexCoord = vin.TexCoord;


	return vout;
}

//------material samplers------
SamplerState gsamPointWrap : register(s0);//static sampler
//------material samplers------
cbuffer CbMaterial : register(b2)
{
bool HaveNormal;
int ShadingModelID;
float ClearCoat;
float ClearCoatRoughness;
};
PixelOut PS(VertexOut pin){
PixelOut pixelOut = (PixelOut)(0.0f);
float4  ConstFloatValue7;
ConstFloatValue7 = float4(0.800000, 0.000000, 0.000000, 1.000000);
pixelOut.finalColor = ConstFloatValue7;
return pixelOut;
}