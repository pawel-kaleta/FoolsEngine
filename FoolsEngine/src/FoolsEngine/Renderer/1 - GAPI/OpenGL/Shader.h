#pragma once

#include "FoolsEngine/Foundation/Memory/DataTypes.h"
#include "FoolsEngine/Foundation/Memory/String.h"
#include "FoolsEngine/Foundation/Memory/Pile.h"

#include "FoolsEngine/Renderer/1 - GAPI/Resource.h"

#include <glad/gl.h>

namespace fe::Render::GAPI::OpenGL
{
	using namespace Descriptors;

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
		constexpr static ObjType Type = ObjType::Shader;

		GLuint mOpenGLID = 0;
		ShaderType mType = ShaderType::None;

		void Init()
		{
			mOpenGLID = 0;
			mType = ShaderType::None;
		}

		void Create(CString source, ShaderType type)
		{
			FE_PROFILER_FUNC();

			mType = type;
			mOpenGLID = glCreateShader(Utils::ShaderTypeToGLEnum(type));

			glShaderSource(mOpenGLID, 1, (GLchar**)source.Data, 0);

			GLint compilation_success;
			{
				FE_PROFILER_SCOPE("OpenGL shader compilation");
				glCompileShader(mOpenGLID);
				glGetShaderiv(mOpenGLID, GL_COMPILE_STATUS, &compilation_success);
			}

			if (compilation_success == GL_FALSE)
			{
				Pile p;

				GLint log_length = 0;
				glGetShaderiv(mOpenGLID, GL_INFO_LOG_LENGTH, &log_length);

				auto info_log = p.Allocate<GLchar>(log_length);
				glGetShaderInfoLog(mOpenGLID, log_length, &log_length, info_log.Elements);

				glDeleteShader(mOpenGLID);

				mOpenGLID = 0;

				FE_LOG_CORE_ERROR("{0}", info_log.Elements);
				FE_CORE_ASSERT(false, "OpenGL shader compilation failed!");

				return;
			}
		}

		void Destroy()
		{
			FE_PROFILER_FUNC();

			glDeleteShader(mOpenGLID);
		}
	};
}