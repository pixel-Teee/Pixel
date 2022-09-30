#type vertex
#version 450 core

layout(location = 0) in vec3 a_Pos;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in int a_EntityID;

layout(std140, binding = 0) uniform UBO{
	mat4 u_Model;
	mat4 u_ViewProjection;	
} ubo;

//uniform mat4 u_Model;
//uniform mat4 u_ViewProjection;

void main()
{
	gl_Position = ubo.u_ViewProjection * ubo.u_Model * vec4(a_Pos, 1.0f);
}

#type fragment
#version 450 core

void main()
{
	
}

