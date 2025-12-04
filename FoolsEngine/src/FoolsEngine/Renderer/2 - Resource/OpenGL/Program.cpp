#include "FE_pch.h"

#include "FoolsEngine\Renderer\2 - Resource\Program.h"
#include "FoolsEngine\Renderer\1 - Description\Library.h"

#include "FoolsEngine\Memory\Scratchpad.h"

namespace fe::Resource
{
	void Program_OpenGL::Create()
	{
		FE_PROFILER_FUNC();

		GLuint ProgramOpenGLID = glCreateProgram();

		std::vector<Shader_OpenGL*>& shaders = *(std::vector<Shader_OpenGL*>*) & Shaders;

		for (int i = 0; i < shaders.size(); i++)
			glAttachShader(ProgramOpenGLID, shaders[i]->ShaderOpenGLID);

		GLint linking_success = 0;
		{
			FE_PROFILER_SCOPE("OpenGL Shader linking");
			glLinkProgram(ProgramOpenGLID);
			glGetProgramiv(ProgramOpenGLID, GL_LINK_STATUS, (int*)&linking_success);
		}

		if (linking_success == GL_FALSE)
		{
			GLint log_length = 0;
			glGetProgramiv(ProgramOpenGLID, GL_INFO_LOG_LENGTH, &log_length);

			Scratchpad sp;
			std::pmr::vector<GLchar> info_log(log_length, &sp);
			glGetProgramInfoLog(ProgramOpenGLID, log_length, &log_length, info_log.data());

			glDeleteProgram(ProgramOpenGLID);
			ProgramOpenGLID = -1;

			FE_LOG_CORE_ERROR("{0}", info_log.data());
			FE_CORE_ASSERT(false, "OpenGL shader program linking failed!");
			return;
		}

		for (int i = 0; i < shaders.size(); i++)
		{
			glDetachShader(ProgramOpenGLID, shaders[i]->ShaderOpenGLID); // do we need this?
		}

		const auto& spec = Description::Library::Get().ProgramSpecs[SpecificationID];

		const auto& uniforms = Description::Library::Get().BufferLayouts[spec.MainUniformsLayoutID];

		for (const auto& uniform : uniforms.Elements)
		{
			GLint location = glGetUniformLocation(ProgramOpenGLID, uniform.Name.c_str());
			BindingLocations.MainUniforms.push_back(location);
		}

		for (const auto& texture_sampler : spec.TextureSamplers)
		{
			GLint location = glGetUniformLocation(ProgramOpenGLID, texture_sampler.Name.c_str());
			BindingLocations.TextureSamplers.push_back(location);
		}
	}

	void Program_OpenGL::Destroy()
	{
		FE_PROFILER_FUNC();

		glDeleteProgram(ProgramOpenGLID);
	}
}