#pragma once

#include "FoolsEngine\Renderer\1 - Description\ShaderInterface.h"

#include <glad\glad.h>

namespace fe::Resource
{
	using namespace Description::ShaderInterface;

	struct ShaderBase
	{
		uint32_t m_SpecificationID;

		virtual void Create(const char* source) = 0;
		virtual void Destroy() = 0;
	};

	struct Shader_OpenGL final : ShaderBase
	{
		GLuint m_OpenGLID;

		virtual void Create(const char* source) override;
		virtual void Destroy() override;
	};
}