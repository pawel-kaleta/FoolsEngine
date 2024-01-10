#type vertex
#version 450 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec4 a_Color;
layout (location = 2) in vec2 a_TexCoord;
layout (location = 3) in float a_TilingFactor;
layout (location = 4) in uint a_TextureSampler;

uniform mat4 u_ViewProjection;

layout (location = 0) out vec4 v_Color;
layout (location = 1) out vec2 v_TexCoord;
layout (location = 2) out flat float v_TilingFactor;
layout (location = 3) out flat uint v_TextureSampler;

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

layout (location = 0) in vec4 v_Color; 
layout (location = 1) in vec2 v_TexCoord;
layout (location = 2) in flat float v_TilingFactor;
layout (location = 3) in flat uint v_TextureSampler;

uniform sampler2D u_Texture[32];

void main()
{
	o_color = v_Color;
	
	switch(v_TextureSampler)
	{
		case  0: o_color *= texture(u_Texture[ 0], v_TexCoord * v_TilingFactor); break;
		case  1: o_color *= texture(u_Texture[ 1], v_TexCoord * v_TilingFactor); break;
		case  2: o_color *= texture(u_Texture[ 2], v_TexCoord * v_TilingFactor); break;
		case  3: o_color *= texture(u_Texture[ 3], v_TexCoord * v_TilingFactor); break;
		case  4: o_color *= texture(u_Texture[ 4], v_TexCoord * v_TilingFactor); break;
		case  5: o_color *= texture(u_Texture[ 5], v_TexCoord * v_TilingFactor); break;
		case  6: o_color *= texture(u_Texture[ 6], v_TexCoord * v_TilingFactor); break;
		case  7: o_color *= texture(u_Texture[ 7], v_TexCoord * v_TilingFactor); break;
		case  8: o_color *= texture(u_Texture[ 8], v_TexCoord * v_TilingFactor); break;
		case  9: o_color *= texture(u_Texture[ 9], v_TexCoord * v_TilingFactor); break;
		case 10: o_color *= texture(u_Texture[10], v_TexCoord * v_TilingFactor); break;
		case 11: o_color *= texture(u_Texture[11], v_TexCoord * v_TilingFactor); break;
		case 12: o_color *= texture(u_Texture[12], v_TexCoord * v_TilingFactor); break;
		case 13: o_color *= texture(u_Texture[13], v_TexCoord * v_TilingFactor); break;
		case 14: o_color *= texture(u_Texture[14], v_TexCoord * v_TilingFactor); break;
		case 15: o_color *= texture(u_Texture[15], v_TexCoord * v_TilingFactor); break;
		case 16: o_color *= texture(u_Texture[16], v_TexCoord * v_TilingFactor); break;
		case 17: o_color *= texture(u_Texture[17], v_TexCoord * v_TilingFactor); break;
		case 18: o_color *= texture(u_Texture[18], v_TexCoord * v_TilingFactor); break;
		case 19: o_color *= texture(u_Texture[19], v_TexCoord * v_TilingFactor); break;
		case 20: o_color *= texture(u_Texture[20], v_TexCoord * v_TilingFactor); break;
		case 21: o_color *= texture(u_Texture[21], v_TexCoord * v_TilingFactor); break;
		case 22: o_color *= texture(u_Texture[22], v_TexCoord * v_TilingFactor); break;
		case 23: o_color *= texture(u_Texture[23], v_TexCoord * v_TilingFactor); break;
		case 24: o_color *= texture(u_Texture[24], v_TexCoord * v_TilingFactor); break;
		case 25: o_color *= texture(u_Texture[25], v_TexCoord * v_TilingFactor); break;
		case 26: o_color *= texture(u_Texture[26], v_TexCoord * v_TilingFactor); break;
		case 27: o_color *= texture(u_Texture[27], v_TexCoord * v_TilingFactor); break;
		case 28: o_color *= texture(u_Texture[28], v_TexCoord * v_TilingFactor); break;
		case 29: o_color *= texture(u_Texture[29], v_TexCoord * v_TilingFactor); break;
		case 30: o_color *= texture(u_Texture[30], v_TexCoord * v_TilingFactor); break;
		case 31: o_color *= texture(u_Texture[31], v_TexCoord * v_TilingFactor); break;
	
	}
	
	if (o_color.w < 0.25)
		discard;
}