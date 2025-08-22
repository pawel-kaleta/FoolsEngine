#type vertex
#version 450 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec3 a_Tangent;
layout (location = 3) in vec2 a_UV0;
layout (location = 4) in vec2 a_UV1;

// Geometry
uniform mat4 u_ViewProjection;
uniform mat4 u_ModelTransform;
uniform vec3 u_Position;

layout (location = 0) out vec2 v_TexCoord;
layout (location = 1) out vec3 v_Normal;

void main()
{
	gl_Position = u_ViewProjection * u_ModelTransform * vec4(a_Position, 1.0);
	
	v_TexCoord = a_UV0;
	v_Normal = a_Normal;
}


#type fragment
#version 450 core

layout (location = 0) out vec4 o_color;
layout (location = 1) out uint o_entityID;

layout (location = 0) in vec2 v_TexCoord;
layout (location = 1) in vec3 v_Normal;

// Textures
uniform sampler2D u_BaseColorMap;
uniform sampler2D u_RoughnessMap;
uniform sampler2D u_MetalnessMap;
uniform sampler2D u_AOMap;
uniform sampler2D u_NormalMap;
uniform sampler2D u_OMRMap;

// Material
uniform vec4 u_BaseColor;
uniform float u_AlphaCutOff;
uniform float u_Roughness;
uniform float u_Metalness;
uniform float u_AO;
uniform bool u_OMRTexturePacking;

// Editor mouse picking
uniform uint u_EntityID;

void main()
{
	vec4 color = texture(u_BaseColorMap, v_TexCoord);
	
	if (color.w < u_AlphaCutOff)
		discard;
	
	o_color = color * vec4(u_BaseColor);
	o_entityID = u_EntityID;
}