#type vertex
#version 450 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoord;

uniform mat4 u_ViewProjection;
uniform uint u_EntityID;
uniform vec3 u_Albedo;
uniform float u_Roughness;
uniform float u_Metalness;
uniform float u_AO;

layout (location = 0) out vec2 v_TexCoord;
layout (location = 1) out vec3 v_Albedo;
layout (location = 2) out float v_Roughness;
layout (location = 3) out float v_Metalness;
layout (location = 4) out float v_AO;
layout (location = 5) out flat uint v_EntityID;

void main()
{
	v_EntityID = u_EntityID;
	v_TexCoord = a_TexCoord;
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
	v_Albedo = vec3(u_Albedo.r * v_Albedo.r, u_Albedo.g * v_Albedo.g, u_Albedo.b * v_Albedo.b);
	v_Roughness = u_Roughness;
	v_Metalness = u_Metalness;
	v_AO = u_AO;
}


#type fragment
#version 450 core

layout (location = 0) out vec4 o_color;
layout (location = 1) out uint o_entityID;

layout (location = 0) in vec2 v_TexCoord;
layout (location = 1) in vec3 v_Albedo;
layout (location = 2) in float v_Roughness;
layout (location = 3) in float v_Metalness;
layout (location = 4) in float v_AO;
layout (location = 5) in flat uint v_EntityID;

uniform sampler2D u_AlbedoMap;
uniform sampler2D u_RoughnessMap;
uniform sampler2D u_MetalnessMap;
uniform sampler2D u_AOMap;

void main()
{
	o_color = vec4(v_Albedo, 1);

	o_entityID = v_EntityID;
}