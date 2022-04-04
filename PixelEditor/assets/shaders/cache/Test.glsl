#type vertex
#version 450 core

layout(location = 0) in vec3 a_Pos;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec3 a_Normal;
layout(location = 3) in int a_EntityID;

layout(location = 0) out vec3 v_Pos;
layout(location = 1) out vec2 v_TexCoord;
layout(location = 2) out vec3 v_Normal;
layout(location = 3) out flat int v_EntityID;

uniform mat4 u_ViewProjection;
uniform mat4 u_Model;

void main(){
vec4 WorldPos = vec4(0.0, 0.0, 0.0, 0.0);
vec3 Normal = vec3(0.0, 0.0, 0.0);
vec2 TexCoord = vec2(0.0, 0.0);
WorldPos = u_Model * vec4(a_Pos, 1.0);
mat3 NormalMatrix = mat3(transpose(inverse(u_Model)));
Normal = NormalMatrix * a_Normal;
TexCoord = a_TexCoord;
v_Pos = WorldPos.xyz;
v_TexCoord = TexCoord;
v_Normal = Normal;
v_EntityID = a_EntityID;
gl_Position = u_ViewProjection * WorldPos;
}
#type fragment
#version 450 core

//------PBR Function------//
const float PI = 3.14159265359;

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

//------PBR Function------//

layout(location = 0) in vec3 v_Pos;
layout(location = 1) in vec2 v_TexCoord;
layout(location = 2) in vec3 v_Normal;
layout(location = 3) in flat int v_EntityID;

layout(location = 0) out vec3 g_Position;
layout(location = 1) out vec3 g_Normal;
layout(location = 2) out vec3 g_Albedo;
layout(location = 3) out vec3 g_RoughnessMetallicEmissive;
layout(location = 4) out flat int g_EntityID;


void main(){
vec3 WorldPos = vec3(0, 0, 0);
vec4  ConstFloatValue1;
ConstFloatValue1 = vec4(0.000000, 0.000000, 0.000000, 0.000000);
vec4  Pos = ConstFloatValue1;
vec4  Albedo = ConstFloatValue1;
float  Roughness = ConstFloatValue1.y;
float  Metallic = ConstFloatValue1.z;
float  Emissive = ConstFloatValue1.w;
vec4  O_WorldPos = vec4(0, 0, 0, 1);
vec4  O_Albedo = vec4(0, 0, 0, 1);
vec4  O_Normal = vec4(0, 0, 0, 1);
float  O_Roughness = 0;
float  O_Metallic = 0;
float  O_Emissive = 0;
Pos = ConstFloatValue1;
Albedo = ConstFloatValue1;
O_Normal = vec4(0, 0, 0, 0);
Roughness = ConstFloatValue1.y;
Metallic = ConstFloatValue1.z;
Emissive = ConstFloatValue1.w;
g_Position = O_WorldPos.xyz;
g_Normal = O_Normal.xyz;
g_Albedo = O_Albedo.xyz;
g_RoughnessMetallicEmissive.x = O_Roughness;
g_RoughnessMetallicEmissive.y = O_Metallic;
g_RoughnessMetallicEmissive.z = O_Emissive;

};

