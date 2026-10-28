#version 450 core
#extension GL_ARB_bindless_texture : require

layout (location = 0) out vec4 o_color;
layout (location = 1) out uint o_entityID;

layout (location = 0) in flat uint QuadIndex;
layout (location = 1) in vec2 TexCoord;

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


void main()
{
	QuadGPU quad = QuadGPUBuffer[QuadIndex];

	o_color = quad.Color;
	o_color *= texture(sampler2D(quad.TextureView), TexCoord * quad.TextureTilingFactor);
	
	if (o_color.w < 0.25)
		discard;

	o_entityID = quad.EntityID;
}