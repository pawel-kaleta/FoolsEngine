#pragma once

#include "FoolsEngine/Renderer/1 - Description/ShaderInterface.h"

#include <glad/glad.h>

namespace fe::Resource
{
	using namespace Description::ShaderInterface;

	struct ShaderBase
	{
		UInt SpecificationID;

		virtual void Create(const char* source) = 0;
		virtual void Destroy() = 0;
	};

	struct Shader_OpenGL final : ShaderBase
	{
		GLuint ShaderOpenGLID;

		virtual void Create(const char* source) override;
		virtual void Destroy() override;
	};
}