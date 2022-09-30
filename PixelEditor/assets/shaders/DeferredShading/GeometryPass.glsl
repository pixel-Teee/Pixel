#type vertex
#version 450 core

layout(location = 0) in vec3 a_Pos;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec3 a_Normal;
layout(location = 3) in vec3 a_Tangent;
layout(location = 4) in vec3 a_Binormal;
layout(location = 5) in int a_EntityID;

layout(location = 0) out vec3 v_WorldPos;
layout(location = 1) out vec3 v_Normal;
layout(location = 2) out vec2 v_TexCoord;
layout(location = 3) out flat int v_EntityID;

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
	//all coordinate are in world pos

	//worldpos
	vec4 WorldPos = u_Model * vec4(a_Pos, 1.0);
	v_WorldPos = WorldPos.xyz;

	//texcoord
	v_TexCoord = a_TexCoord;

	//normal
	mat3 NormalMatrix = mat3(transpose(inverse(u_Model)));
	v_Normal = NormalMatrix * a_Normal;

	v_EntityID = a_EntityID;

	gl_Position = u_ViewProjection * WorldPos;
}

#type fragment
#version 450 core
layout(location = 0) out vec3 g_Position;
layout(location = 1) out vec3 g_Normal;
layout(location = 2) out vec3 g_Albedo;
//roughness metallic emissive
layout(location = 3) out vec3 g_RoughnessMetallicEmissive;
layout(location = 4) out flat int g_EntityID;

layout(location = 0) in vec3 v_WorldPos;
layout(location = 1) in vec3 v_Normal;
layout(location = 2) in vec2 v_TexCoord;
layout(location = 3) in flat int v_EntityID;

uniform sampler2D tex_Albedo;
uniform sampler2D tex_roughness;
uniform sampler2D tex_metallic;
uniform sampler2D tex_normalMap;
uniform sampler2D tex_emissive;

vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(tex_normalMap, v_TexCoord).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(v_WorldPos);
    vec3 Q2  = dFdy(v_WorldPos);
    vec2 st1 = dFdx(v_TexCoord);
    vec2 st2 = dFdy(v_TexCoord);

    vec3 N   = normalize(v_Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

void main()
{
	//calculate world pos
	g_Position = v_WorldPos;
	//calculate world normal
	g_Normal = (getNormalFromMap() + 1.0) / 2.0;
	//albedo
	g_Albedo.xyz = texture(tex_Albedo, v_TexCoord).rgb;
	//roughness
	g_RoughnessMetallicEmissive.r = texture(tex_roughness, v_TexCoord).r;
	//metallic
	g_RoughnessMetallicEmissive.g = texture(tex_metallic, v_TexCoord).r;
	//emissive
	g_RoughnessMetallicEmissive.b = texture(tex_emissive, v_TexCoord).r;
	g_EntityID = v_EntityID;
}

