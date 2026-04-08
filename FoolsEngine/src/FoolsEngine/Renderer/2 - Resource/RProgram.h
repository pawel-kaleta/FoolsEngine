#pragma once

#include "RShader.h"

#include "FoolsEngine/Assets/Asset.h"

#include "FoolsEngine/Renderer/1 - Description/ShaderInterface.h"
#include "FoolsEngine/Renderer/1 - Description/GAPIType.h"

#include <glad/glad.h>

namespace fe::Resource
{
	struct ProgramBase
	{
		U32 SpecificationID;

		virtual void Create() = 0;
		virtual void Destroy() = 0;
	};

	template <GAPIType::ValueType GAPI>
	struct RProgram;

	template <>
	struct RProgram<GAPIType::OpenGL> final : public ProgramBase
	{
		GLuint ProgramOpenGLID;
		Splice<GLuint> ShaderOpenGLIDs;

		struct
		{
			Splice<GLint> MainUniforms;
			Splice<GLint> TextureSamplers;
		} BindingLocations;

		virtual void Create() override;
		virtual void Destroy() override;
	};

	using RProgram_OpenGL = RProgram<GAPIType::OpenGL>;
}