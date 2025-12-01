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

		struct Uniform
		{
			Uniform(const std::string& name, Data::Type type, uint32_t count = 1)
				: Name(name), Type(type), Count(count) {
			}

			Uniform()
				: Type(Description::Data::Type::None), Count(0) {
			}

			const Data::Structure	GetStructure()	const { return Data::StructureInType(Type); }
			const Data::Primitive	GetPrimitive()	const { return Data::PrimitiveInType(Type); }
			const size_t			GetSize()		const { return Data::SizeOfType(Type) * Count; }

			Data::Type	Type;
			std::string	Name;
			uint32_t	Count;
		};

		struct TextureSampler
		{
			TextureSampler()
				: Spec(), Count(0) { Spec.Init(); }

			TextureSampler(const std::string& name, Texture::Specification spec, uint32_t count = 1)
				: Name(name), Spec(spec), Count(count) { }

			Texture::Specification Spec;
			std::string	Name;
			uint32_t Count;
		};

		struct UniformBufferSampler { };
		struct DynamicBufferSampler { };

		struct Var
		{
			Var(const std::string& name, Data::Type type)
				: Name(name), Type(type) { }

			Var()
				: Type(Data::Type::None) {}

			Data::Type	Type;
			std::string	Name;
		};

		struct Specification
		{
			ShaderType Type;


			std::vector<Uniform>				Uniforms;
			std::vector<TextureSampler>			TextureSamplers;
			std::vector<UniformBufferSampler>	UniformBufferSamplers;
			std::vector<DynamicBufferSampler>	DynamicBufferSamplers;

			Buffer::Layout InputLayout;
			Buffer::Layout OutputLayout;
		};

		struct ProgramSpecification
		{
			uint32_t VertexShaderSpecificationID;
			uint32_t FragmentShaderSpecificationID;

			bool VertexOutputCapture;
		};
	}
}