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
	vec3 T = normalize(vec3(u_ModelTransform * vec4(a_Tangent, 0.0)));
	vec3 N = normalize(vec3(u_ModelTransform * vec4(a_Normal , 0.0)));
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);
	v_TBN = mat3(T, B, N);

	gl_Position = u_ViewProjection * u_ModelTransform * vec4(a_Position, 1.0);

	v_FragPos = vec3(u_ModelTransform * vec4(a_Position, 1.0));
	
	v_TexCoord = a_UV0;
}
/////////////////////////////////////////////////


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
uniform sampler2D u_ORMMap;

// Material
uniform float u_AlphaCutOff;
uniform bool u_ORMTexturePacking;

// Scene
uniform vec3 u_MainLightDir;
uniform vec3 u_MainLightColor;
uniform float u_MainLightIntensity;
uniform vec3 u_AmbientLight;
uniform float u_AmbientLightIntensity;
uniform vec3 u_CameraPosition;

// Editor mouse picking
uniform uint u_EntityID;

const float PI = 3.14159265359;

float NDF(vec3 N, vec3 H, float a)
{
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float nom    = a2;
    float denom  = (NdotH2 * (a2 - 1.0) + 1.0);
    denom        = PI * denom * denom;
	
    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float k)
{
    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return nom / denom;
}
  
float GeometrySmith(vec3 N, vec3 V, vec3 L, float k)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, k);
    float ggx2 = GeometrySchlickGGX(NdotL, k);
	
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (vec3(1.0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main()
{
	vec4 c = texture(u_BaseColorMap, v_TexCoord);
	
	if (c.w < u_AlphaCutOff)
		discard;

	float o;
	float m;
	float a;

	if (u_ORMTexturePacking)
	{
		vec3 orm = texture(u_ORMMap, v_TexCoord).rgb;
		o = orm.r;
		a = orm.g;
		m = orm.b;
		
		o = 1.0 - o;
	}
	else
	{
		o = texture(u_AOMap, v_TexCoord).r;
		a = texture(u_RoughnessMap, v_TexCoord).r;
		m = texture(u_MetalnessMap, v_TexCoord).r;
	}

	vec3 L = normalize(-u_MainLightDir);
	vec3 V = normalize(u_CameraPosition - v_FragPos);
	vec3 H = normalize(V + L);

	vec3 N = texture(u_NormalMap, v_TexCoord).rgb;
	N = N * 2.0 - 1.0;
	N = normalize(v_TBN * N);

	float cosTheta = max(dot(L, H), 0.0);

	vec3 F0 = vec3(0.04);
	F0      = mix(F0, c.rgb, m);
	vec3 F	= fresnelSchlick(cosTheta, F0);

	float D = NDF(N, H, a);       

	float k = ((a + 1.0)*(a + 1.0)) / 8.0;
	float G = GeometrySmith(N, V, L, k);

	vec3 BRDF_nom = D * F * G;
	float BRDF_denom = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0)  + 0.0001;
	vec3 BRDF = BRDF_nom / BRDF_denom;

	vec3 kS = F;
	vec3 kD = (vec3(1.0) - kS) * (1.0 - m);

	float NdotL = max(dot(N, L), 0.0); 
	vec3 Ld = kD * c.rgb / PI;
    vec3 Ls = BRDF;
	vec3 Lo = (Ld + Ls) * u_MainLightColor * u_MainLightIntensity * NdotL;

	vec3 ambient_light = o * u_AmbientLight * u_AmbientLightIntensity * c.rgb;

	vec3 color = Lo + ambient_light;

	o_color = vec4(color, 1.0);
	
	o_entityID = u_EntityID;
}