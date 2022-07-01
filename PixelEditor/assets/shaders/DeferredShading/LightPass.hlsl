/*
	Light Pass Shading
*/

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

//------gbuffer texture------
Texture2D gBufferPosition : register(t0);
Texture2D gBufferNormal : register(t1);
Texture2D gBufferAlbedo : register(t2);
Texture2D gBufferRoughnessMetallicEmissive : register(t3);
//------gbuffer texture------

//------gbuffer sampler------
SamplerState gsamPointWrap : register(s0);//static sampler
//------gbuffer sampler------

struct Light
{
	float3 Position;
	float3 Direction;
	float3 Color;
	float Intensity;
	float FallOffRadius;
	float InnerAngle;
	float OuterAngle;
	float LengthLight;
	float Radius;
};

#define MAXLIGHTS 16
#define PI 3.1415926535

cbuffer LightPass : register(b1)
{
	float3 CameraPos;
	Light lights[16];
};

float D_GGX(float NoH, float Roughness)
{
	float a = NoH * Roughness;
	float k = Roughness / (1.0 - NoH * NoH + a * a);
	return k * k * (1.0 / PI);
}

float V_SmithGGXCorrelated(float NoV, float NoL, float Roughness)
{
	float a2 = Roughness * Roughness;
	float GGXV = NoL * sqrt(NoV * NoV * (1.0 - a2) + a2);
	float GGXL = NoL * sqrt(NoL * NoL * (1.0 - a2) + a2);
	return 0.5 / (GGXV + GGXL);
}

float3 F_Shlick(float u, float3 f0, float f90) {
	return f0 + (float3(f90, f90, f90)-f0) * pow(1.0 - u, 5.0);
}

float Fd_Lambert() {
	return 1.0 / PI;
}

PixelOut PS(VertexOut pin)
{
	PixelOut pixelOut = (PixelOut)(0.7f);
	//get the gbuffer texture 's value
	float3 PosW = gBufferPosition.Sample(gsamPointWrap, pin.TexCoord).xyz;
	float3 NormalW = (gBufferNormal.Sample(gsamPointWrap, pin.TexCoord) * 2.0f - 1.0f).xyz;
	float3 Albedo = gBufferAlbedo.Sample(gsamPointWrap, pin.TexCoord).xyz;
	float Roughness = gBufferRoughnessMetallicEmissive.Sample(gsamPointWrap, pin.TexCoord).x;
	float Metallic = gBufferRoughnessMetallicEmissive.Sample(gsamPointWrap, pin.TexCoord).y;
	float Emissive = gBufferRoughnessMetallicEmissive.Sample(gsamPointWrap, pin.TexCoord).z;

	float3 N = normalize(NormalW);
	float3 V = normalize(CameraPos - PosW);

	float3 f0 = float3(0.04, 0.04, 0.04);
	f0 = lerp(f0, Albedo, Metallic);
	
	float3 Lo = float3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < 1; ++i)
	{
		float3 L = lights[i].Position - PosW;
		float distance = length(L);
		L = normalize(L);
		if (distance < lights[i].FallOffRadius)
		{
			//calculate lights
			float3 H = normalize(V + L);
			float NoV = abs(dot(N, V)) + 1e-5;
			float NoL = clamp(dot(N, L), 0.0, 1.0);
			float NoH = clamp(dot(N, H), 0.0, 1.0);
			float LoH = clamp(dot(L, H), 0.0, 1.0);

			//perceptually linear roughness to roughness
			float D = D_GGX(NoH, Roughness);
			float3 F = F_Shlick(LoH, f0, 1.0);
			float V = V_SmithGGXCorrelated(NoV, NoL, Roughness);

			//specular brdf
			float3 Fr = (D * V) * F / (4.0 * NoV * NoL + 0.0001);

			float3 kS = F;
			float3 kD = float3(1.0, 1.0, 1.0) - kS;
			kD *= 1.0 - Metallic;

			//diffuse brdf
			float3 Fd = Albedo * Fd_Lambert();

			Lo += (kD * Fd + Fr) * NoL * lights[i].Color;
		}
	}

	pixelOut.Color = float4(Lo, 1.0);


	return pixelOut;
}
