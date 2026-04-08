#include "FE_pch.h"

#include "Utils.h"

#include "FoolsEngine/Renderer/1 - Description/Library.h"
#include "FoolsEngine/Renderer/2 - Resource/RShader.h"
#include "FoolsEngine/Foundation/Memory/Pile.h"

namespace fe::Resource
{
	void RShader<GAPIType::OpenGL>::Create(String source)
	{
		FE_PROFILER_FUNC();

		const auto& spec = Description::Library::Get().ShaderSpecs[SpecificationID];

		ShaderOpenGLID = glCreateShader(Utils::ShaderTypeToGLEnum(spec.Type));
		glShaderSource(ShaderOpenGLID, 1, (GLchar**) & source.Buffer.Elements, 0);

		GLint compilation_success;
		{
			FE_PROFILER_SCOPE("OpenGL shader compilation");
			glCompileShader(ShaderOpenGLID);
			glGetShaderiv(ShaderOpenGLID, GL_COMPILE_STATUS, &compilation_success);
		}

		if (compilation_success == GL_FALSE)
		{
			Pile p;

			GLint log_length = 0;
			glGetShaderiv(ShaderOpenGLID, GL_INFO_LOG_LENGTH, &log_length);

			auto info_log = p.Allocate<GLchar>(log_length);
			glGetShaderInfoLog(ShaderOpenGLID, log_length, &log_length, info_log.Elements);

			glDeleteShader(ShaderOpenGLID);

			ShaderOpenGLID = 0;

			FE_LOG_CORE_ERROR("{0}", info_log.Elements);
			FE_CORE_ASSERT(false, "OpenGL shader compilation failed!");

			return;
		}
	}

	void RShader<GAPIType::OpenGL>::Destroy()
	{
		FE_PROFILER_FUNC();

		glDeleteShader(ShaderOpenGLID);
	}
}