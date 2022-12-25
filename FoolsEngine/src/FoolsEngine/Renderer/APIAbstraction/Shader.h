#pragma once

#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "Buffers.h"

namespace fe
{
	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void UploadUniform(const std::string& name, void* dataPointer, SDType type, uint32_t count = 1, bool transpose = false) = 0;

		virtual const std::string& GetName() const = 0;
		virtual const uint32_t& GetProgramID() const = 0;

		static Ref<Shader> Create(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource);
	private:

	};

}
