struct VertexIn
{
	float3 PosL : POSITION;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
};

struct PixelOut
{
	float4 Color : SV_Target;
};

cbuffer CbPass : register(b1)
{
	float4x4 gViewProjection;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout = (VertexOut)0.0f;

	//to clip space
	vout.PosH = mul(float4(vin.PosL, 1.0f), gViewProjection);
	vout.PosW = vin.PosL;
	return vout;
}

Texture2D equirectangularMap : register(t0);

SamplerState gsamPointWrap : register(s0);

#define PI 3.1415926535

float2 SampleSphericalMap(float3 coord)
{
	float theta = acos(coord.y);
	float phi = atan2(coord.x, coord.z);
	phi += (phi < 0) ? 2 * PI : 0;

	float u = phi * 0.1591f;
	float v = theta * 0.3183f;
	return float2(u, v);
}

PixelOut PS(VertexOut pin)
{
	PixelOut pixelOut = (PixelOut)(0.0f);

	float3 envColor = equirectangularMap.Sample(gsamPointWrap, SampleSphericalMap(normalize(pin.PosW))).xyz;
	envColor = envColor / (envColor + float3(1.0f, 1.0f, 1.0f));
	float gamma = 1.0f / 2.0f;

	envColor = pow(envColor, float3(gamma, gamma, gamma));

	//test
	pixelOut.Color.xyz = envColor;
	pixelOut.Color.w = 1.0f;

	return pixelOut;
}