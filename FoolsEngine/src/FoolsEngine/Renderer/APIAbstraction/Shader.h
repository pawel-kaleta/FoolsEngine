#pragma once

#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "Buffers.h"

namespace fe
{
	class Uniform
	{
	public:
		Uniform(const std::string& name, ShaderData::Type type)
			: m_Name(name), m_Type(type) {}

		Uniform()
			: m_Name(""), m_Type(ShaderData::Type::None) {}

		inline const std::string& GetName() const { return m_Name; }
		inline const ShaderData::Type& GetType() const { return m_Type; }
		inline const ShaderData::Structure GetStructure() const { return ShaderData::StructureInType(m_Type); }
		inline const ShaderData::Primitive GetPrimitive() const { return ShaderData::PrimitiveInType(m_Type); }
		inline const uint32_t GetSize() const { return SizeOfType(m_Type); }
	private:
		std::string m_Name;
		ShaderData::Type m_Type;
	};

	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void UploadUniform(const Uniform& uniform, void* dataPointer, uint32_t count = 1, bool transpose = false) = 0;

		virtual const std::string& GetName() const = 0;
		virtual const uint32_t& GetProgramID() const = 0;

		static Ref<Shader> Create(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource);
	private:

	};

}
