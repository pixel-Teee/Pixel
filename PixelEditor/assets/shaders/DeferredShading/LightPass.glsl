#type vertex
#version 450 core

layout(location = 0) in vec3 a_Pos;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in int a_EntityID;

/*
layout(std140, binding = 0) uniform UBO{
	mat4 u_Model;
	mat4 u_ViewProjection;	
} ubo;
*/

uniform mat4 u_Model;
uniform mat4 u_ViewProjection;

void main()
{
	gl_Position = u_ViewProjection * u_Model * vec4(a_Pos, 1.0f);
}

#type fragment
#version 450 core

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);

uniform sampler2D g_Position;
uniform sampler2D g_Normal;
uniform sampler2D g_Albedo;
uniform sampler2D g_RoughnessMetallicEmissive;

/*
layout(std140, binding = 1) uniform Light{
	vec3 position;
	vec3 color;

	float constant;
	float linear;
	float quadratic;
} light;

layout(std140, binding = 2) uniform Camera{
	vec2 gScreenSize;
	vec3 camPos;
} camera;
*/

struct Light{
	vec3 position;
	vec3 color;

	float constant;
	float linear;
	float quadratic;
};

uniform Light light;

uniform	vec2 gScreenSize;
uniform	vec3 camPos;

vec2 CalcTexCoord()
{
	return gl_FragCoord.xy / gScreenSize; 
}

float CalcAttenuation(vec3 WorldPos, vec3 Normal)
{
	float distance = length(light.position - WorldPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance +
	light.quadratic * (distance * distance));
	
	return attenuation;
}

void main()
{
	//sampler texture value
	vec2 TexCoord = CalcTexCoord();
	vec3 WorldPos = texture(g_Position, TexCoord).xyz;
	vec3 Normal = (texture(g_Normal, TexCoord).xyz * 2.0) - 1.0f;
	vec3 Albedo = texture(g_Albedo, TexCoord).xyz;
	float roughness = texture(g_RoughnessMetallicEmissive, TexCoord).r;
	float Metallic = texture(g_RoughnessMetallicEmissive, TexCoord).g;
	float Emissive = texture(g_RoughnessMetallicEmissive, TexCoord).b;

	vec3 N = normalize(Normal);
	vec3 V = normalize(camPos - WorldPos);

	//PBR
	vec3 F0 = vec3(0.04);
	F0 = mix(F0, Albedo, Metallic);

	vec3 Lo = vec3(0.0);
	
	vec3 L = normalize(light.position - WorldPos);
	vec3 H = normalize(V + L);
	float attenuation = CalcAttenuation(WorldPos, N);
	vec3 radiance = light.color * attenuation;

	//Cook-Torrance BRDF
	float NDF = DistributionGGX(N, H, roughness);   
    float G   = GeometrySmith(N, V, L, roughness);      
    vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

	vec3 numerator    = NDF * G * F; 
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;

	vec3 kS = F;

	vec3 kD = vec3(1.0) - kS;

	kD *= 1.0 - Metallic;

	float NdotL = max(dot(N, L), 0.0);

	Lo += (kD * Albedo / PI + specular) * radiance * NdotL;

	//ambient
	vec3 ambient = vec3(0.03) * Albedo;

	vec3 color = (ambient + Lo);
	
	//HDR
	color = color / (color + vec3(1.0));

	color = pow(color, vec3(1.0 / 2.2));

	//calculate pixel value
	gl_FragColor =  vec4(color, attenuation);
}

//GGX
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

//geometry item
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}



