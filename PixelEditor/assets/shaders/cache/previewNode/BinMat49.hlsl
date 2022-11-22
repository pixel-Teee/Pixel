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
float4  a;
bool HaveNormal;
int ShadingModelID;
float ClearCoat;
float ClearCoatRoughness;
};
PixelOut PS(VertexOut pin){
PixelOut pixelOut = (PixelOut)(0.0f);
float4  ConstFloatValue148;
ConstFloatValue148 = float4(0.000000, 0.000000, 0.000000, 1.000000);
float4  ConstFloatValue125;
ConstFloatValue125 = float4(25.120001, 25.120001, 0.000000, 1.000000);
float2  TexCoordinate15 = float2(0, 0);
TexCoordinate15 = pin.TexCoord;
float4  MulInputA18 = ConstFloatValue125;
float4  MulInputB19 = float4(TexCoordinate15.x, TexCoordinate15.y, TexCoordinate15.y, TexCoordinate15.y);
float4  MulOutput20 = float4(0, 0, 0, 1);
MulOutput20 = MulInputA18 * MulInputB19;
float4  SinInputValue27 = MulOutput20;
float4  SinOutputValue28 = float4(0, 0, 0, 1);
SinOutputValue28 = sin(SinInputValue27);
float4  InputValue33 = SinOutputValue28;
float  OutputValue34 = 0;
OutputValue34 = InputValue33.r;
float4  InputValue38 = SinOutputValue28;
float  OutputValue39 = 0;
OutputValue39 = InputValue38.g;
float  DotA45 = OutputValue34;
float  DotB46 = OutputValue39;
float4  Scalar47 = float4(0, 0, 0, 1);
Scalar47 = dot(DotA45, DotB46);
float  InputValue80 = Scalar47.x;
float  CeilOutput81 = 0;
CeilOutput81 = ceil(InputValue80);
float4  X86 = ConstFloatValue148;
float4  Y87 = a;
float4  S88 = float4(CeilOutput81, CeilOutput81, CeilOutput81, CeilOutput81);
float4  LerpOutput89 = float4(0, 0, 0, 1);
LerpOutput89 = lerp(X86, Y87, S88);
pixelOut.finalColor = LerpOutput89;
return pixelOut;
}