#version 450 core
#extension GL_ARB_bindless_texture : require

layout (location = 0) uniform uint RootDataIndex;

struct RootData
{
	uint SceneDataIndex;
};

layout(std140, binding = 0) readonly buffer RootDataStream
{
    RootData RootDataBuffer[];
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

layout(std140) struct SceneData
{
	mat4x4 VPMatrix;
	DirectionalLight MainLight;
	vec4 AmbientLight;
	vec3 CameraPosition;
};

layout(std140, binding = 1) readonly buffer SceneDataStream
{
    SceneData SceneDataBuffer[];
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

layout(std140, binding = 2) readonly buffer QuadGPUStream
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
	QuadIndex = gl_VertexID / 4;

	QuadGPU quad = QuadGPUBuffer[QuadIndex];

	vec4 vertex_position = quad_vertex_positions[gl_VertexID % 4];
	vertex_position.y *= quad.AspecRatio;
	vertex_position = quad.Transform * vertex_position;

	RootData root_data = RootDataBuffer[RootDataIndex];
	uint scene_data_index = root_data.SceneDataIndex;
	SceneData scene_data = SceneDataBuffer[scene_data_index];

	gl_Position = scene_data.VPMatrix * vertex_position;

	TexCoord = texture_coords[gl_VertexID % 4];
}