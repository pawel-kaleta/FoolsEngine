#pragma once

#include "Shader.h"

#include "FoolsEngine/Assets/Asset.h"

#include "FoolsEngine/Renderer/1 - Description/ShaderInterface.h"

#include <glad/glad.h>

namespace fe::Resource
{
	struct ProgramBase
	{
		Splice<AssetID> Shaders;
		U32 SpecificationID;

		virtual void Create() = 0;
		virtual void Destroy() = 0;
	};

	struct Program_OpenGL final : ProgramBase
	{
		GLuint ProgramOpenGLID;

		struct
		{
			Splice<GLint> MainUniforms;
			Splice<GLint> TextureSamplers;
		} BindingLocations;

		virtual void Create() override;
		virtual void Destroy() override;
	};

	//struct Program_Vulkan final : ProgramBase
	//{
	//	virtual void Create() override {};
	//	virtual void Destroy() override {};
	//};
}