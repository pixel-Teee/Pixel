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
TextureCube IrradianceMap : register(t4);
//------gbuffer texture------

//------gbuffer sampler------
SamplerState gsamPointWrap : register(s0);//static sampler
//------gbuffer sampler------

struct Light
{
	float3 Position;
	float3 Direction;
	float3 Color;
	float Radius;
};

#define PI 3.1415926535

cbuffer LightPass : register(b1)
{
	float3 CameraPos;
	int PointLightNumber;
	int DirectLightNumber;
	int SpotLightNumber;
	Light lights[16];
};

float DistributionGGX(float NoH, float Roughness)
{
	float a = Roughness * Roughness;
	float a2 = a * a;
	float NoH2 = NoH * NoH;

	float num = a2;
	float denom = (NoH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float num = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return num / denom;
}
float GeometrySmith(float NoV, float NoL, float Roughness)
{
	float ggx2 = GeometrySchlickGGX(NoV, Roughness);
	float ggx1 = GeometrySchlickGGX(NoL, Roughness);

	return ggx1 * ggx2;
}

float3 F_Shlick(float u, float3 f0, float f90) {
	return f0 + (float3(f90, f90, f90) - f0) * pow(1.0 - u, 5.0);
}

float Fd_Lambert() {
	return 1.0 / PI;
}

float3 AccumulatePointLight(float NoV, float NoL, float NoH, float LoH, Light light, float Roughness, float3 f0, float3 Albedo)
{
	float3 Lo = float3(0.0f, 0.0f, 0.0f);

	//perceptually linear roughness to roughness
	float D = DistributionGGX(NoH, Roughness);
	float3 F = F_Shlick(LoH, f0, 1.0);
	float V = GeometrySmith(NoV, NoL, Roughness);

	//specular brdf
	float3 Fr = (D * V) * F / (4.0 * NoV * NoL + 0.0001);

	//diffuse brdf
	float3 Fd = Albedo * Fd_Lambert();

	return (Fr + Fd) * light.Color * NoL;
}

PixelOut PS(VertexOut pin)
{
	PixelOut pixelOut = (PixelOut)(0.7f);
	//get the gbuffer texture 's value
	float3 NormalW = gBufferNormal.Sample(gsamPointWrap, pin.TexCoord).xyz;
	bool NeedDiscard = (NormalW.x == 0.0f && NormalW.y == 0.0f && NormalW.z == 0.0f);
	if (NeedDiscard)
		discard;
	NormalW = NormalW * 2.0f - 1.0f;
	float3 PosW = gBufferPosition.Sample(gsamPointWrap, pin.TexCoord).xyz;
	float3 Albedo = pow(gBufferAlbedo.Sample(gsamPointWrap, pin.TexCoord).xyz, 2.2);
	float Roughness = gBufferRoughnessMetallicEmissive.Sample(gsamPointWrap, pin.TexCoord).x;
	float Metallic = gBufferRoughnessMetallicEmissive.Sample(gsamPointWrap, pin.TexCoord).y;
	float Emissive = gBufferRoughnessMetallicEmissive.Sample(gsamPointWrap, pin.TexCoord).z;

	float3 N = normalize(NormalW);
	float3 V = normalize(CameraPos - PosW);

	float3 f0 = float3(0.04, 0.04, 0.04);//non-metal's base reflectance
	f0 = lerp(f0, Albedo, Metallic);//metal's reflectance
	
	float3 Lo = float3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < PointLightNumber; ++i)
	{
		float3 L = lights[i].Position - PosW;//point light direction
		float distance = length(L);
		L = normalize(L);
		if (distance < lights[i].Radius)
		{
			//calculate lights
			float3 H = normalize(V + L);
			float NoV = abs(dot(N, V)) + 1e-5;
			float NoL = clamp(dot(N, L), 0.0, 1.0);
			float NoH = clamp(dot(N, H), 0.0, 1.0);
			float LoH = clamp(dot(L, H), 0.0, 1.0);

			Lo += AccumulatePointLight(NoV, NoL, NoH, LoH, lights[i], Roughness, f0, Albedo);
		}
	}

	//direct light
	for (int i = PointLightNumber; i < PointLightNumber + DirectLightNumber; ++i)
	{
		float3 L = normalize(-lights[i].Direction);

		float3 H = normalize(V + L);
		float NoV = abs(dot(N, V)) + 1e-5;
		float NoL = clamp(dot(N, L), 0.0, 1.0);
		float NoH = clamp(dot(N, H), 0.0, 1.0);
		float LoH = clamp(dot(L, H), 0.0, 1.0);

		float D = DistributionGGX(NoH, Roughness);
		float3 F = F_Shlick(LoH, f0, 1.0);
		float V = GeometrySmith(NoV, NoL, Roughness);

		//specular brdf
		float3 Fr = (D * V) * F / (4.0 * NoV * NoL + 0.0001);

		//diffuse brdf
		float3 Fd = Albedo * Fd_Lambert();

		Lo += (Fr + Fd) * lights[i].Color * NoL;
	}

	//------IBL------
	float3 kS = F_Shlick(max(dot(N, V), 0.0f), f0, 1.0);
	float3 kD = 1.0f - kS;
	kD *= 1.0 - Metallic;
	float3 Irradiance = IrradianceMap.Sample(gsamPointWrap, N).xyz;
	float3 diffuse = Irradiance * Albedo;
	float3 ambient = kD * diffuse;
	//------IBL------

	Lo += ambient;

	Lo = Lo / (Lo + float3(1.0f, 1.0f, 1.0f));

	float Gamma = 1.0f / 2.2f;

	Lo = pow(Lo, float3(Gamma, Gamma, Gamma));

	pixelOut.Color = float4(Lo, 1.0f);

	return pixelOut;
}
