#pragma once

#include "Data.h"
#include "Buffer.h"
#include "Texture.h"

#include "FoolsEngine/Application/UUID.h"
#include "FoolsEngine/Foundation/Memory/DataTypes.h"

namespace fe
{
	namespace Description::ShaderInterface
	{
		FE_DECLARE_ENUM(ShaderType, None, Vertex, Fragment);

		struct VertexAttribute
		{
			String Name;
			Data::Type Type;
		};

		struct TextureSampler
		{
			String Name;
			U32 TextureArchetypeID = -1;
		};

		struct UniformBufferSampler { };
		struct DynamicBufferSampler { };

		struct FragmentAttribute
		{
			String Name;
			Texture::Format Format;
		};

		struct Uniform
		{
			String Name;
			Data::Type Type;
			U32 Count;

			Data::Primitive Primitive() const { return Data::PrimitiveInType(Type); }
			Data::Structure Structure() const { return Data::StructureInType(Type); }
			UInt Size() const { return Data::SizeOfType(Type); }
			UInt ComponentCount() const
			{
				bool ifDouble = Primitive() == Data::Primitive::Double;
				return Size() / (4 * (1 + (int)ifDouble));
			}
		};

		struct ProgramSpecification
		{
			UUID UUID = fe::UUID();

			Splice<Uniform> Uniforms;
			Splice<VertexAttribute> VertexAttributes;
			Splice<TextureSampler> TextureSamplers;
			Splice<UniformBufferSampler> UniformBufferSamplers;
			Splice<DynamicBufferSampler> DynamicBufferSamplers;
		};
	}
}