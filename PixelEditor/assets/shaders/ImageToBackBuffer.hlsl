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

VertexOut VS(VertexIn vin)
{
	VertexOut vout = (VertexOut)0.0f;

	//to clip space
	vout.PosH = float4(vin.PosL, 1.0f);
	vout.TexCoord = vin.TexCoord;

	return vout;
}

Texture2D Image : register(t0);

SamplerState gsamPointWrap : register(s0);

PixelOut PS(VertexOut pin)
{
	PixelOut pixelOut = (PixelOut)(0.0f);

	float3 Color = Image.Sample(gsamPointWrap, pin.TexCoord);

	pixelOut.Color = float4(Color, 1.0f);

	return pixelOut;
}