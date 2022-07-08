struct VertexIn
{
	float3 PosL : POSITION;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
};

cbuffer cbPerObject : register(b0)
{
	float4x4 gWorld;//world matrix
};

cbuffer cbPass : register(b1)
{
	float4x4 gViewProjection;
};

struct PixelOut
{
	float4 Color : SV_Target;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout = (VertexOut)0.0f;

	//to clip space
	vout.PosH = mul(float4(vin.PosL, 1.0f), gViewProjection);

	return vout;
}

PixelOut PS(VertexOut pin)
{
	PixelOut pixelOut = (PixelOut)(0.0f);
	pixelOut.Color = float4(1.0f, 1.0f, 1.0f, 1.0f);

	return pixelOut;
}