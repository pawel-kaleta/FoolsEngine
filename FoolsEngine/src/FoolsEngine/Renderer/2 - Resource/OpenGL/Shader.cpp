#include "FE_pch.h"

#include "FoolsEngine\Renderer\2 - Resource\Shader.h"
#include "FoolsEngine\Renderer\1 - Description\Library.h"

#include "FoolsEngine\Memory\Scratchpad.h"

namespace fe::Resource
{
	GLenum Shader_OpenGL::ShaderTypeToGLenum(ShaderType type)
	{
		switch (type)
		{
		case ShaderType::None:		return GL_NONE;
		case ShaderType::Vertex:	return GL_VERTEX_SHADER;
		case ShaderType::Fragment:	return GL_FRAGMENT_SHADER;
		default:
			FE_LOG_CORE_ERROR("Unrecognized shader type");
			return GL_NONE;
		}
	};

	void Shader_OpenGL::Create(const char* source)
	{
		const auto& spec = Description::Library::Get().ShaderSpecs[m_SpecificationID];

		m_OpenGLID = glCreateShader(ShaderTypeToGLenum(spec.Type));
		glShaderSource(m_OpenGLID, 1, &source, 0);

		GLint compilation_success;
		{
			FE_PROFILER_SCOPE("OpenGL shader compilation");
			glCompileShader(m_OpenGLID);
			glGetShaderiv(m_OpenGLID, GL_COMPILE_STATUS, &compilation_success);
		}

		if (compilation_success == GL_FALSE)
		{
			Scratchpad sp;

			GLint log_length = 0;
			glGetShaderiv(m_OpenGLID, GL_INFO_LOG_LENGTH, &log_length);

			std::pmr::vector<GLchar> info_log(log_length, &sp);
			glGetShaderInfoLog(m_OpenGLID, log_length, &log_length, info_log.data());

			glDeleteShader(m_OpenGLID);

			m_OpenGLID = 0;

			FE_LOG_CORE_ERROR("{0}", info_log.data());
			FE_CORE_ASSERT(false, "OpenGL shader compilation failed!");

			return;
		}
	}
}