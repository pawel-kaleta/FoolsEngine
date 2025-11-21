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

out vec2 v_TexCoord;
out vec3 v_FragPos;
out mat3 v_TBN;

void main()
{
	gl_Position = u_ViewProjection * u_ModelTransform * vec4(a_Position, 1.0);
	
	v_TexCoord = a_UV0;
}


#type fragment
#version 450 core

out vec4 o_color;
out uint o_entityID;

in vec2 v_TexCoord;
in vec3 v_FragPos;
in mat3 v_TBN;

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

// Scene
uniform vec3 u_MainLightDir;
uniform vec3 u_MainLightColor;
uniform vec3 u_AmbientLight;
uniform vec3 u_CameraPosition;

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