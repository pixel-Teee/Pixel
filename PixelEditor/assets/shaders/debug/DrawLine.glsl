#type vertex
#version 450 core

layout(location = 0) in vec3 a_Pos;

uniform mat4 u_ViewProjection;

void main()
{
	gl_Position = u_ViewProjection * vec4(a_Pos, 1.0f);
}

#type fragment
#version 450 core

void main()
{
	gl_FragColor = vec4(0.0f, 1.0f, 0.0, 1.0f);
}