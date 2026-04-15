#pragma once

#include "RShader.h"

#include "FoolsEngine/Renderer/1 - Description/ShaderInterface.h"
#include "FoolsEngine/Renderer/1 - Description/GAPIType.h"

#include <glad/glad.h>

namespace fe::Resource
{
	struct RProgram
	{
		U32 SpecificationID;
		Splice<UInt> VertexAttributesStreamIDs;
		UInt PipelineStateID;

		virtual void Create() = 0;
		virtual void Destroy() = 0;
	};

	struct RProgram_OpenGL final : public RProgram
	{
		GLuint OpenGLID;
		Splice<GLuint> ShaderOpenGLIDs;

		struct
		{
			Splice<GLint> Uniforms;
			Splice<GLint> TextureSamplers;
			Splice<GLint> UniformBufferSamplers; // not handled yet
			Splice<GLint> DynamicBufferSamplers; // not handled yet
			Splice<Splice<GLint>> VertexAttributesLocations;
		} BindingLocations;

		virtual void Create() override;
		virtual void Destroy() override;
	};
}