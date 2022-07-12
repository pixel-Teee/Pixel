struct VertexIn
{
	float3 PosL : POSITION;
	float2 TexCoord : TEXCOORD;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

struct PixelOut
{
	float4 Color : SV_Target;
};

cbuffer cb : register(b1)
{
	float exposure;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout = (VertexOut)0.0f;

	//to clip space
	vout.PosH = float4(vin.PosL, 1.0f);
	vout.TexCoord = vin.TexCoord;

	return vout;
}

Texture2D Scene : register(t0);
Texture2D BloomBlur : register(t1);

SamplerState gsamPointWrap : register(s0);

PixelOut PS(VertexOut pin)
{
	PixelOut pixelOut = (PixelOut)(0.0f);

	const float invGamma = 1.0f / 2.2f;

	float3 hdrColor = Scene.Sample(gsamPointWrap, pin.TexCoord).xyz;

	float3 bloomColor = BloomBlur.Sample(gsamPointWrap, pin.TexCoord).xyz;

	//additive blending
	hdrColor += bloomColor;

	//tone mapping
	float3 result = float3(1.0f, 1.0f, 1.0f) - exp(-hdrColor * 1.0f);

	//gamma correct
	result = pow(result, float3(invGamma, invGamma, invGamma));

	pixelOut.Color = float4(result, 1.0f);

	return pixelOut;
}