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

layout (location = 0) out vec2 v_TexCoord;
layout (location = 1) out mat3 v_TBN;
layout (location = 2) out vec2 v_FragPos;

void main()
{
	vec3 B = cross(a_Normal, a_Tangent);
	mat3 TBN = transpose(mat3(a_Tangent, B, a_Normal));


	gl_Position = u_ViewProjection * u_ModelTransform * vec4(a_Position, 1.0);

	v_FragPos = vec3(u_ModelTransform * vec4(a_Position, 1.0));
	
	v_TexCoord = a_UV0;
	v_Normal = a_Normal;
}


#type fragment
#version 450 core

layout (location = 0) out vec4 o_color;
layout (location = 1) out uint o_entityID;

layout (location = 0) in vec2 v_TexCoord;
layout (location = 1) in mat3 v_TBN

// Textures
uniform sampler2D u_BaseColorMap;
uniform sampler2D u_RoughnessMap;
uniform sampler2D u_MetalnessMap;
uniform sampler2D u_AOMap;
uniform sampler2D u_NormalMap;
uniform sampler2D u_OMRMap;

// Material
uniform vec3 u_BaseColor;
uniform float u_Roughness;
uniform float u_Metalness;
uniform float u_AO;
uniform bool u_OMRTexturePacking;

// Scene
uniform vec3 u_MainLight;
uniform vec3 u_CameraPosition;

// Editor mouse picking
uniform uint u_EntityID;

void main()
{
	vec3 lightDir = normalize(-u_MainLight);
	vec3 lightColor = vec3(23.47, 21.31, 20.79);

	vec3 normal = texture(normalMap, v_TexCoord).rgb;
	normal = normal * 2.0 - 1.0;
	normal = normalize(v_TBN * normal);



	o_color = vec4(u_BaseColor, 1) * texture(u_BaseColorMap, v_TexCoord);
	o_entityID = u_EntityID;
}