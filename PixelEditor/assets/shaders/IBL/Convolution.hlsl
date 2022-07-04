struct VertexIn
{
	float3 PosL : POSITION;
};

struct VertexOut
{
	float3 PosW : POSITION;
	float4 PosH : SV_POSITION;
};

cbuffer CbPass : register(b1)
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

	vout.PosW = vin.PosL;
	//to clip space
	vout.PosH = mul(float4(vin.PosL, 1.0f), gViewProjection);
	
	return vout;
}

TextureCube environmentMap : register(t0);

SamplerState gsamPointWrap : register(s0);

#define PI 3.1415926535

PixelOut PS(VertexOut pin)
{
	PixelOut pixelOut = (PixelOut)(0.0f);

	float3 N = normalize(pin.PosW);

	float3 irradiance = float3(0.0f, 0.0f, 0.0f);

	//convolution code
	float3 up = float3(0.0f, 1.0f, 0.0f);
	float3 right = normalize(cross(up, N));
	up = normalize(cross(N, right));

	float3 sampleDelta = 0.025f;
	float nrSamples = 0.0f;

	for (float phi = 0.0f; phi < 2.0f * PI; phi += sampleDelta)
	{
		for (float theta = 0.0f; theta < 0.5f * PI; theta += sampleDelta)
		{
			//spherical to catersian(in tangent space)
			float3 tangentSample = float3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));

			//tangent space to world space
			float3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;

			irradiance += environmentMap.Sample(gsamPointWrap, sampleVec).xyz * cos(theta) * sin(theta);

			nrSamples = nrSamples + 1.0f;
		}
	}

	irradiance = PI * irradiance * (1.0 / float(nrSamples));
	pixelOut.Color = float4(irradiance, 1.0f);
	return pixelOut;
}