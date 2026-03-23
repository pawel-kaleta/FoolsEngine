#pragma once

#include "Data.h"
#include "Buffer.h"
#include "Texture.h"

#include "FoolsEngine/Application/UUID.h"

namespace fe
{
	namespace Description::ShaderInterface
	{
		FE_DECLARE_ENUM(ShaderType, None, Vertex, Fragment);

		struct TextureSampler
		{
			String Name;
			UUID UUID = fe::UUID();
			U32 TextureArchetypeID = -1;
		};

		struct UniformBufferSampler { };
		struct DynamicBufferSampler { };

		struct Specification
		{
			UUID UUID = fe::UUID();
			ShaderType Type = ShaderType::None;

			U32 InputLayoutID = -1;
			U32 OutputLayoutID = -1;
			U32 MainUniformsLayoutID = -1;

			Splice<U32> TextureSamplerIDs;
			Splice<U32> UniformBufferSamplerIDs;
			Splice<U32> DynamicBufferSamplerIDs;
		};

		struct ProgramSpecification
		{
			UUID UUID = fe::UUID();

			U32 VertexInputLayoutID = -1;
			U32 VertexOutputLayoutID = -1; // needed only for VertexOutputCapture into buffer
			U32 FragmentOutputLayoutID = -1;
			U32 MainUniformsLayoutID = -1;

			Splice<U32> TextureSamplerIDs;
			Splice<U32> UniformBufferSamplerIDs;
			Splice<U32> DynamicBufferSamplerIDs;
		};
	}
}