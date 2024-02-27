#pragma once

#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "FoolsEngine\Renderer\1 - Primitives\GDIType.h"
#include "FoolsEngine\Renderer\1 - Primitives\Uniform.h"
#include "FoolsEngine\Renderer\1 - Primitives\ShaderTextureSlot.h"

namespace fe
{
	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void UploadUniform(const Uniform& uniform, void* dataPointer, uint32_t count = 1, bool transpose = false) = 0;
		virtual void BindTextureSlot(const ShaderTextureSlot& textureSlot, uint32_t* rendererTextureSlot, uint32_t count = 1) = 0;
		virtual void BindTextureSlot(const ShaderTextureSlot& textureSlot, uint32_t rendererTextureSlot) = 0;

		virtual const std::string& GetName() const = 0;
		virtual const uint32_t& GetProgramID() const = 0;

		static Scope<Shader> Create(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource, GDIType GDI);
		static Scope<Shader> Create(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource);
		static Scope<Shader> Create(const std::string& name, const std::string& shaderSource, GDIType GDI);
		static Scope<Shader> Create(const std::string& name, const std::string& shaderSource);
		static Scope<Shader> Create(const std::string& filePath, GDIType GDI);
		static Scope<Shader> Create(const std::string& filePath);
	private:

	};

	
}
