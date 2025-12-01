#pragma once

#include <string>

#include "Data.h"
#include "Texture.h"
#include "Buffer.h"

namespace fe
{
	namespace Description::ShaderInterface
	{
		FE_DECLARE_ENUM(ShaderType, None, Vertex, Fragment);

		struct TextureSampler
		{
			TextureSampler()
				: Spec() { Spec.Init(); }

			TextureSampler(const std::string& name, Texture::Specification spec, uint32_t count = 1)
				: Name(name), Spec(spec) { }

			Texture::Specification Spec;
			std::string	Name;
		};

		struct UniformBufferSampler { };
		struct DynamicBufferSampler { };

		struct Specification
		{
			ShaderType Type;

			Buffer::Layout InputLayout;
			Buffer::Layout OutputLayout;
			Buffer::Layout Uniforms;

			std::vector<TextureSampler>			TextureSamplers;
			std::vector<UniformBufferSampler>	UniformBufferSamplers;
			std::vector<DynamicBufferSampler>	DynamicBufferSamplers;

		};

		struct ProgramSpecification
		{
			Buffer::Layout VertexInputLayout;
			Buffer::Layout VertexOutputLayout; // needed only for VertexOutputCapture into buffer
			Buffer::Layout FragmentOutputLayout;
			Buffer::Layout Uniforms;

			std::vector<TextureSampler>			TextureSamplers;
			std::vector<UniformBufferSampler>	UniformBufferSamplers;
			std::vector<DynamicBufferSampler>	DynamicBufferSamplers;

			bool VertexOutputCapture; //TO DO: this is most likely not enough
		};
	}
}