#type vertex
#version 450 core

layout(location = 0) in vec3 a_Pos;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in int a_EntityID;

layout(location = 0) out vec3 v_WorldPos;
layout(location = 1) out vec3 v_Normal;
layout(location = 2) out vec2 v_TexCoord;
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

	v_WorldPos = WorldPos.xyz;
	v_Normal = Normal;
	v_TexCoord = TexCoord;
	v_EntityID = a_EntityID;
	gl_Position = u_ViewProjection * WorldPos;
}

#type fragment
#version 450 core

layout(location = 0) in vec3 v_WorldPos;
layout(location = 1) in vec3 v_Normal;
layout(location = 2) in vec2 v_TexCoord;
layout(location = 3) in flat int v_EntityID;

layout(location = 0) out vec4 Color;

void main(){
	

};
