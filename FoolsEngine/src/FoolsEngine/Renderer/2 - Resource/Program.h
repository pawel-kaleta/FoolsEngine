#pragma once

#include "Shader.h"

#include "FoolsEngine\Renderer\1 - Description\ShaderInterface.h"

#include <glad\glad.h>

namespace fe::Resource
{
	struct ProgramBase
	{
		uint32_t SpecificationID;
		std::vector<ShaderBase*> Shaders; // make this an id in resource manager

		virtual void Create() = 0;
		virtual void Destroy() = 0;

		virtual void UploadUniform(const std::pmr::string& uniformName, const void* data) const = 0;
		virtual void UploadUniform(size_t uniformIndex, const void* data) const = 0;
	};

	struct Program_OpenGL final : ProgramBase
	{
		GLuint ProgramOpenGLID;

		struct
		{
			std::vector<GLint> MainUniforms;
			std::vector<GLint> TextureSamplers;
		} BindingLocations;

		virtual void Create() override;
		virtual void Destroy() override;
		virtual void UploadUniform(const std::pmr::string& uniformName, const void* data) const override;
		virtual void UploadUniform(size_t uniformIndex, const void* data) const override;
	};
}