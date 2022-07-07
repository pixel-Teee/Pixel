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
	float4x4 gLightSpaceMatrix;
};

//struct PixelOut
//{
//	float4 Color : SV_Target;
//};

VertexOut VS(VertexIn vin)
{
	VertexOut vout = (VertexOut)0.0f;

	//to clip space
	vout.PosH = mul(mul(float4(vin.PosL, 1.0f), gWorld), gLightSpaceMatrix);

	return vout;
}

void PS(VertexOut pin)
{
	//only write to depth buffer
}