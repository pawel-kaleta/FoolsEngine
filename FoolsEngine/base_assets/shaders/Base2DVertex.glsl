#version 450 core
#extension GL_ARB_bindless_texture : require


layout(std140, binding = 0) uniform RootData
{
    uint FirsQuadIndex;
};

struct DirectionalLight
{
	float Direction_x;
	float Direction_y;
	float Direction_z;
	float Intensity;
	float Color_r;
	float Color_g;
	float Color_b;
	float zPadding_1;
};

layout(std140, binding = 1) uniform SceneData
{
	mat4x4 VPMatrix;
	DirectionalLight MainLight;
	vec4 AmbientLight;
	vec3 CameraPosition;
};

struct QuadGPU
{
	mat4x4	Transform;
	vec4	Color;
	uvec2	TextureView;
	float	AspecRatio;
	float	TextureTilingFactor;
	uint	EntityID;
	float	Padding;
	float	Padding2;
	float	Padding3;
	float	Padding4;
};

layout(std140, binding = 0) readonly buffer QuadGPUStream
{
    QuadGPU QuadGPUBuffer[];
};


layout (location = 0) out flat uint QuadIndex;
layout (location = 1) out vec2 TexCoord;


const vec4 quad_vertex_positions[4] = {
	{ -0.5f, -0.5f, 0.0f, 1.0f },
	{  0.5f, -0.5f, 0.0f, 1.0f },
	{  0.5f,  0.5f, 0.0f, 1.0f },
	{ -0.5f,  0.5f, 0.0f, 1.0f }
};

const vec2 texture_coords[4] = {
	{ 0.0f, 0.0f },
	{ 1.0f, 0.0f },
	{ 1.0f, 1.0f },
	{ 0.0f, 1.0f }
};

void main()
{
	QuadIndex = FirsQuadIndex + gl_VertexID / 4;

	QuadGPU quad = QuadGPUBuffer[QuadIndex];

	vec4 vertex_position = quad_vertex_positions[gl_VertexID % 4];
	vertex_position.y *= quad.AspecRatio;
	vertex_position = quad.Transform * vertex_position;

	gl_Position = VPMatrix * vertex_position;

	TexCoord = texture_coords[gl_VertexID % 4];
}