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
vec4  ConstFloatValue3;
ConstFloatValue3 = vec4(0.000000, 0.000000, 0.000000, 0.000000);
vec4  MulInputA5 = ConstFloatValue1;
vec4  MulInputB7 = ConstFloatValue3;
vec4  MulOutput8 = vec4(0, 0, 0, 1);
MulOutput8 = MulInputA5 * MulInputB7;
vec4  Pos = vec4(0, 0, 0, 0);
vec4  Albedo = vec4(0, 0, 0, 0);
float  Roughness = 0;
float  Metallic = 0;
float  Emissive = 0;
vec4  O_WorldPos = vec4(0, 0, 0, 1);
vec4  O_Albedo = vec4(0, 0, 0, 1);
vec4  O_Normal = vec4(0, 0, 0, 1);
float  O_Roughness = 0;
float  O_Metallic = 0;
float  O_Emissive = 0;
O_WorldPos = vec4(v_Pos, 1.0);
O_Albedo = vec4(0, 0, 0, 0);
O_Normal = MulOutput8;
O_Roughness = 0;
O_Metallic = 0;
O_Emissive = 0;
g_Position = O_WorldPos.xyz;
g_Normal = O_Normal.xyz;
g_Albedo = O_Albedo.xyz;
g_RoughnessMetallicEmissive.x = O_Roughness;
g_RoughnessMetallicEmissive.y = O_Metallic;
g_RoughnessMetallicEmissive.z = O_Emissive;

};

