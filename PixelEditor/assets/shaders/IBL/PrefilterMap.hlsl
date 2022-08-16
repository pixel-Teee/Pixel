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
	float gRoughness;
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

float DistributionGGX(float3 N, float3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0f);
	float NdotH2 = NdotH * NdotH;

	float nom = a2;
	float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
	denom = PI * denom / denom;

	return nom / denom;
}

float RadicalInverse_VdC(uint bits)
{
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

float2 Hammersley(uint i, uint N)
{
	return float2(float(i) / float(N), RadicalInverse_VdC(i));
}

float3 ImportanceSampleGGX(float2 Xi, float3 N, float roughness)
{
	float a = roughness * roughness;

	float phi = 2.0f * PI * Xi.x;
	float cosTheta = sqrt((1.0f - Xi.y) / (1.0f + (a * a - 1.0f) * Xi.y));
	float sinTheta = sqrt(1.0f - cosTheta * cosTheta);

	float3 H;
	H.x = cos(phi) * sinTheta;
	H.y = sin(phi) * sinTheta;
	H.z = cosTheta;

	float3 up = abs(N.z) < 0.999 ? float3(0.0f, 0.0f, 1.0f) : float3(1.0f, 0.0f, 0.0f);
	float3 tangent = normalize(cross(up, N));
	float3 bitangent = cross(N, tangent);

	float3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
	return normalize(sampleVec);
}

PixelOut PS(VertexOut pin)
{
	PixelOut pixelOut = (PixelOut)(0.0f);

	float3 N = normalize(pin.PosW);
	float3 R = N;
	float3 V = R;

	uint SAMPLE_COUNT = 1024u;
	float3 prefilteredColor = float3(0.0f, 0.0f, 0.0f);
	float totalWeight = 0.0f;

	for (uint i = 0; i < SAMPLE_COUNT; ++i)
	{
		//generate a sample vector that's biased towards the preferred alignment direction
		float2 Xi = Hammersley(i, SAMPLE_COUNT);
		float3 H = ImportanceSampleGGX(Xi, N, gRoughness);
		float3 L = normalize(2.0f * dot(V, H) * H - V);

		float NdotL = max(dot(N, L), 0.0f);

		if (NdotL > 0.0f)
		{
			//float D = DistributionGGX(N, H, roughness);
			//float NdotH = max(dot(N, H), 0.0);
			//float HdotV = max(dot(H, V), 0.0);
			//float pdf = D * NdotH / (4.0 * HdotV) + 0.0001;

			//float resolution = 512.0; // resolution of source cubemap (per face)
			//float saTexel = 4.0 * PI / (6.0 * resolution * resolution);
			//float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);

			//float mipLevel = roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);

			//prefilteredColor += textureLod(environmentMap, L, mipLevel).rgb * NdotL;
			//totalWeight += NdotL;
			prefilteredColor += environmentMap.Sample(gsamPointWrap, L).rgb * NdotL;
			totalWeight += NdotL;
		}
	}

	prefilteredColor = prefilteredColor / totalWeight;

	pixelOut.Color = float4(prefilteredColor, 1.0f);

	return pixelOut;
}