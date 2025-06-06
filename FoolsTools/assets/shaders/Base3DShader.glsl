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

// Material
uniform vec3 u_BaseColor;
uniform float u_Roughness;
uniform float u_Metalness;
uniform float u_AO;

// Editor mouse picking
uniform uint u_EntityID;

layout (location = 0) out vec2 v_TexCoord;
layout (location = 1) out vec3 v_BaseColor;
layout (location = 2) out float v_Roughness;
layout (location = 3) out float v_Metalness;
layout (location = 4) out float v_AO;
layout (location = 5) out flat uint v_EntityID;
layout (location = 6) out vec3 v_Normal;

void main()
{
	v_EntityID = u_EntityID;
	v_TexCoord = a_UV0;
	
	gl_Position = u_ViewProjection * u_ModelTransform * vec4(a_Position, 1.0);

	v_BaseColor = u_BaseColor;
	v_Roughness = u_Roughness;
	v_Metalness = u_Metalness;
	v_AO = u_AO;
	v_Normal = a_Normal;
}


#type fragment
#version 450 core

layout (location = 0) out vec4 o_color;
layout (location = 1) out uint o_entityID;

layout (location = 0) in vec2 v_TexCoord;
layout (location = 1) in vec3 v_BaseColor;
layout (location = 2) in float v_Roughness;
layout (location = 3) in float v_Metalness;
layout (location = 4) in float v_AO;
layout (location = 5) in flat uint v_EntityID;
layout (location = 6) in vec3 v_Normal;

uniform sampler2D u_BaseColorMap;
uniform sampler2D u_RoughnessMap;
uniform sampler2D u_MetalnessMap;
uniform sampler2D u_AOMap;
uniform sampler2D u_NormalMap;

void main()
{
	
	o_color = vec4(v_BaseColor, 1) * texture(u_BaseColorMap, v_TexCoord);
	//o_color = vec4(v_BaseColor, 1);
	o_entityID = v_EntityID;
}