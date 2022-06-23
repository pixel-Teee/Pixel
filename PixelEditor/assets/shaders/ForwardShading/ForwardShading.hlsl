//------mesh constants------
cbuffer cbPerObject : register(b0)
{
	float4x4 gWorld;//world matrix
};
//------mesh constants------

cbuffer cbPass : register(b1)
{
	float4x4 gView;
	float4x4 gProjection;
	float4x4 gViewProjection;
	float3 cameraPos;
	//------Light------
	float3 LightPosition;
	float3 LightColor;
	//------Light------
};

struct VertexIn
{
	float3 PosL : POSITION;
	float2 TexCoord : TEXCOORD;
	float3 NormalL : NORMAL;
	int Editor : EDITOR;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float2 TexCoord : TEXCOORD;
	float3 NormalW : NORMAL;
	float4x4 Test : TEXCOORD2;
	int Editor : EDITOR;
};

struct PixelOut
{
	float4 color1 : SV_Target;
	int color2 : SV_Target1;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout = (VertexOut)0.0f;

	//to world space
	float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
	vout.PosW = posW.xyz;

	vout.NormalW = mul(vin.NormalL, (float3x3)gWorld);

	//homogeneous clipping
	//vout.PosH = mul(posW, gView);
	vout.PosH = mul(posW, gViewProjection);

	vout.Test = mul(gView, gProjection);

	vout.TexCoord = vin.TexCoord;

	vout.Editor = vin.Editor;
	return vout;
}

//------material constants------
cbuffer cbMaterail : register(b2)
{
	float4x4 MatTransform;
};
//------material constants------

//------material texture------
Texture2D gDiffuseMap : register(t0);
//------material texture------

//------material samplers------
SamplerState gsamPointWrap : register(s0);
//------material samplers------

PixelOut PS(VertexOut pin)
{
	pin.NormalW = normalize(pin.NormalW);
	
	float3 toLight = normalize(LightPosition - pin.PosW);

	float3 ambient = LightColor * max(dot(pin.NormalW, toLight), 0.0f);
	//float3 toEyeW = normalize(gEyePosW - pin.PosW);

	PixelOut pixelOut;

	//return float4(ambient, 1.0f);
	pixelOut.color1 = float4(ambient, 1.0f);
	pixelOut.color2 = pin.Editor;

	return pixelOut;
}