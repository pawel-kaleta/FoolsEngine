#pragma once

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
			TextureSampler();

			Texture::Specification Spec;
			std::pmr::string Name;
		};

		struct UniformBufferSampler { };
		struct DynamicBufferSampler { };

		struct Specification
		{
			Specification();

			ShaderType Type;

			uint32_t InputLayoutID;
			uint32_t OutputLayoutID;
			uint32_t MainUniformsLayoutID;

			std::pmr::vector<uint32_t> TextureSamplerIDs;
			std::pmr::vector<uint32_t> UniformBufferSamplerIDs;
			std::pmr::vector<uint32_t> DynamicBufferSamplerIDs;
		};

		struct ProgramSpecification
		{
			ProgramSpecification();

			uint32_t VertexInputLayoutID;
			uint32_t VertexOutputLayoutID; // needed only for VertexOutputCapture into buffer
			uint32_t FragmentOutputLayoutID;
			uint32_t MainUniformsLayoutID;

			std::pmr::vector<TextureSampler> TextureSamplers;
			std::pmr::vector<uint32_t> UniformBufferSamplerIDs;
			std::pmr::vector<uint32_t> DynamicBufferSamplerIDs;

			bool VertexOutputCapture; //TO DO: this is most likely not enough
		};
	}
}