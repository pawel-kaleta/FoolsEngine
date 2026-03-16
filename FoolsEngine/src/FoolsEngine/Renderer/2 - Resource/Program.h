#pragma once

#include "Shader.h"

#include "FoolsEngine/Assets/Asset.h"

#include "FoolsEngine/Renderer/1 - Description/ShaderInterface.h"

#include <glad/glad.h>

namespace fe::Resource
{
	struct ProgramBase
	{
		uint32_t SpecificationID;
		std::vector<AssetID> Shaders;

		virtual void Create() = 0;
		virtual void Destroy() = 0;
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
	};

	struct Program_Vulkan final : ProgramBase
	{
		virtual void Create() override {};
		virtual void Destroy() override {};
	};
}