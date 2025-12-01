#pragma once

#include "FoolsEngine\Renderer\1 - Description\ShaderInterface.h"

using GLuint = uint32_t;

namespace fe::Resource
{
	struct ShaderBase
	{
		Description::ShaderInterface::Specification Specification; // Just an ID?
	};

	struct Shader_OpenGL : ShaderBase
	{
		GLuint OpenGLID;
	};

	struct Shader_Vulkan : ShaderBase { };

	namespace Shader
	{
		using OpenGL = Shader_OpenGL;
		using Vulkan = Shader_Vulkan;
	}
}