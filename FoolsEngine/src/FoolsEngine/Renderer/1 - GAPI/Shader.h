#pragma once

#include "FoolsEngine/Foundation/Memory/DataTypes.h"
#include "FoolsEngine/Foundation/Memory/String.h"
#include "FoolsEngine/Foundation/Memory/Pile.h"

#include "Descriptors.h"

#include <glad/glad.h>

namespace fe::GAPI::Resource
{
	using namespace Descriptors::Shader;

	namespace Utils
	{
		GLenum ShaderTypeToGLEnum(ShaderType type)
		{
			switch (type)
			{
			case ShaderType::None:
				FE_CORE_ASSERT(false, "Not specified Shader Type");
				return GL_NONE;
			case ShaderType::Vertex:	return GL_VERTEX_SHADER;
			case ShaderType::Fragment:	return GL_FRAGMENT_SHADER;
			case ShaderType::Compute:	return GL_COMPUTE_SHADER;
			default:
				FE_LOG_CORE_ERROR("Unrecognized shader type");
				return GL_NONE;
			}
		}
	}

	struct Shader
	{
		GLuint OpenGLID = 0;
		ShaderType Type = ShaderType::None;

		void Create(CString source, ShaderType type)
		{
			FE_PROFILER_FUNC();

			Type = type;
			OpenGLID = glCreateShader(Utils::ShaderTypeToGLEnum(type));

			glShaderSource(OpenGLID, 1, (GLchar**)source.Data, 0);

			GLint compilation_success;
			{
				FE_PROFILER_SCOPE("OpenGL shader compilation");
				glCompileShader(OpenGLID);
				glGetShaderiv(OpenGLID, GL_COMPILE_STATUS, &compilation_success);
			}

			if (compilation_success == GL_FALSE)
			{
				Pile p;

				GLint log_length = 0;
				glGetShaderiv(OpenGLID, GL_INFO_LOG_LENGTH, &log_length);

				auto info_log = p.Allocate<GLchar>(log_length);
				glGetShaderInfoLog(OpenGLID, log_length, &log_length, info_log.Elements);

				glDeleteShader(OpenGLID);

				OpenGLID = 0;

				FE_LOG_CORE_ERROR("{0}", info_log.Elements);
				FE_CORE_ASSERT(false, "OpenGL shader compilation failed!");

				return;
			}
		}

		void Destroy()
		{
			FE_PROFILER_FUNC();

			glDeleteShader(OpenGLID);
		}
	};
}