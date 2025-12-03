#include "FE_pch.h"

#include "FoolsEngine\Renderer\2 - Resource\Program.h"

#include "FoolsEngine\Memory\Scratchpad.h"

namespace fe::Resource
{
	void Program_OpenGL::Create()
	{
		GLuint m_OpenGLID = glCreateProgram();

		std::vector<Shader_OpenGL*>& shaders = *(std::vector<Shader_OpenGL*>*) & m_Shaders;

		for (int i = 0; i < shaders.size(); i++)
			glAttachShader(m_OpenGLID, shaders[i]->m_OpenGLID);

		GLint linking_success = 0;
		{
			FE_PROFILER_SCOPE("OpenGL Shader linking");
			glLinkProgram(m_OpenGLID);
			glGetProgramiv(m_OpenGLID, GL_LINK_STATUS, (int*)&linking_success);
		}

		if (linking_success == GL_FALSE)
		{
			GLint log_length = 0;
			glGetProgramiv(m_OpenGLID, GL_INFO_LOG_LENGTH, &log_length);

			Scratchpad sp;
			std::pmr::vector<GLchar> info_log(log_length, &sp);
			glGetProgramInfoLog(m_OpenGLID, log_length, &log_length, info_log.data());

			glDeleteProgram(m_OpenGLID);
			m_OpenGLID = -1;

			FE_LOG_CORE_ERROR("{0}", info_log.data());
			FE_CORE_ASSERT(false, "OpenGL shader program linking failed!");
			return;
		}

		for (int i = 0; i < shaders.size(); i++)
		{
			glDetachShader(m_OpenGLID, shaders[i]->m_OpenGLID); // do we need this?
		}
	}
}