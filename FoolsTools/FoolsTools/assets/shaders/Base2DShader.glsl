#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TilingFactor;
layout(location = 4) in uint a_TextureSampler;

uniform mat4 u_ViewProjection;

out vec4 v_Color;
out vec2 v_TexCoord;
out float v_TilingFactor;
out uint v_TextureSampler;

void main()
{
	v_Color = a_Color;
	v_TexCoord = a_TexCoord;
	v_TilingFactor = a_TilingFactor;
	v_TextureSampler = a_TextureSampler;
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}


#type fragment
#version 450 core

layout(location = 0) out vec4 o_color;

in vec4 v_Color; 
in vec2 v_TexCoord;
in float v_TilingFactor;
flat in uint v_TextureSampler;

uniform sampler2D u_Texture[32];

void main()
{
	o_color = texture(u_Texture[v_TextureSampler], v_TexCoord * v_TilingFactor) * v_Color;
}