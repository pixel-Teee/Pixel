struct VertexIn
{
	float3 PosL : POSITION;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float3 PosL : POSITION;
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
	vout.PosL = vin.PosL;
	
	return vout;
}

TextureCube environmentMap : register(t0);

SamplerState gsamPointWrap : register(s0);

PixelOut PS(VertexOut pin)
{
	PixelOut pixelOut = (PixelOut)(0.0f);

	float3 envColor = environmentMap.Sample(gsamPointWrap, pin.PosL);

	pixelOut.Color = float4(envColor, 1.0f);

	return pixelOut;
}