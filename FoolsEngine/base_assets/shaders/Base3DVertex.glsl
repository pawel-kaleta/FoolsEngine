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