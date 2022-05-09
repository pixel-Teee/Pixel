#type vertex
#version 450 core

layout(location = 0) in vec3 a_Pos;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in int a_EntityID;

uniform mat4 u_ViewProjection;
uniform mat4 u_Model;

void main()
{
	gl_Position = u_ViewProjection * u_Model * vec4(a_Pos, 1.0f);
}

#type fragment
#version 450 core

void main()
{
	gl_FragColor = vec4(0.4f, 0.0f, 0.0f, 0.4f);
}

