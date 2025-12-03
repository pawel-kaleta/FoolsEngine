#pragma once

#include "Shader.h"

#include "FoolsEngine\Renderer\1 - Description\ShaderInterface.h"

#include <glad\glad.h>

namespace fe::Resource
{
	struct ProgramBase
	{
		uint32_t m_SpecificationID;
		std::vector<ShaderBase*> m_Shaders;

		virtual void Create() = 0;
	};

	struct Program_OpenGL final : ProgramBase
	{
		GLuint m_OpenGLID;

		virtual void Create() override;
	};
}