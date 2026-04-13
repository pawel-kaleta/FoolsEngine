#include "FE_pch.h"

#include "FoolsEngine/Foundation/Memory/Pile.h"

#include "FoolsEngine/Assets/AssetAccessors.h"
#include "FoolsEngine/Foundation/Utils/Context.h"

#include "FoolsEngine/Renderer/1 - Description/Library.h"
#include "FoolsEngine/Renderer/2 - Resource/RProgram.h"
#include "FoolsEngine/Renderer/5 - Representation/Shader.h"

namespace fe::Resource
{
	void RProgram_OpenGL::Create()
	{
		FE_PROFILER_FUNC();

		GLuint ProgramOpenGLID = glCreateProgram();

		for (int i = 0; i < ShaderOpenGLIDs.Count; i++)
			glAttachShader(ProgramOpenGLID, ShaderOpenGLIDs[i]);

		GLint linking_success = 0;
		{
			FE_PROFILER_SCOPE("OpenGL Shader linking");
			glLinkProgram(ProgramOpenGLID);
			glGetProgramiv(ProgramOpenGLID, GL_LINK_STATUS, (int*)&linking_success);
		}

		Pile p;
		if (linking_success == GL_FALSE)
		{
			GLint log_length = 0;
			glGetProgramiv(ProgramOpenGLID, GL_INFO_LOG_LENGTH, &log_length);

			auto info_log = p.Allocate<GLchar>(log_length);
			glGetProgramInfoLog(ProgramOpenGLID, log_length, &log_length, info_log.Elements);

			glDeleteProgram(ProgramOpenGLID);
			ProgramOpenGLID = -1;

			FE_LOG_CORE_ERROR("{0}", info_log.Elements); // is this null terminated?
			FE_CORE_ASSERT(false, "OpenGL shader program linking failed!");
			return;
		}

		for (int i = 0; i < ShaderOpenGLIDs.Count; i++)
		{
			glDetachShader(ProgramOpenGLID, ShaderOpenGLIDs[i]); // do we need this?
		}


		const auto& library = Description::Library::Get();

		const auto& spec = library.ProgramSpecs[(UInt)SpecificationID];
		
		const auto& uniforms = spec.Uniforms;

		BindingLocations.Uniforms = Context::Allocators::Default->Allocate<GLint>(uniforms.Count);
		for (UInt i = 0; i < uniforms.Count; i++)
		{
			const auto& name = uniforms.Elements[i].Name;
			GLint location = glGetUniformLocation(ProgramOpenGLID, (GLchar*)name.Data());

			BindingLocations.Uniforms[i] = location;
		}

		BindingLocations.TextureSamplers = Context::Allocators::Default->Allocate<GLint>(spec.TextureSamplers.Count);
		for (UInt i = 0; i < spec.TextureSamplers.Count; i++)
		{
			const auto& texture_sampler = spec.TextureSamplers[i];
			const auto& name = texture_sampler.Name;
			GLint location = glGetUniformLocation(ProgramOpenGLID, (GLchar*)name.Data());

			BindingLocations.TextureSamplers[i] = location;
		}

		glGetAttribLocation
	}

	void RProgram_OpenGL::Destroy()
	{
		FE_PROFILER_FUNC();

		glDeleteProgram(OpenGLID);

		Context::Allocators::Default->Deallocate(BindingLocations.MainUniforms);
		Context::Allocators::Default->Deallocate(BindingLocations.TextureSamplers);
	}
}