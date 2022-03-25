#type vertex
#version 330 core

layout(location = 0) in vec3 a_Pos;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in int a_EntityID;

uniform mat4 u_ViewProjection;
uniform mat4 u_Model;

out vec2 v_TexCoord;

void main()
{
	v_TexCoord = a_TexCoord;
	gl_Position = u_ViewProjection * u_Model * vec4(a_Pos, 1.0f);
}

#type fragment
#version 330 core

in vec2 v_TexCoord;

uniform sampler2D m_quad;

void main()
{
	gl_FragColor = texture(m_quad, v_TexCoord);
}

