#pragma once

#include "FoolsEngine/Foundation/Memory/DataTypes.h"
#include "FoolsEngine/Foundation/Memory/Pile.h"
#include "Shader.h"
#include "Descriptors.h"

#include <glad/glad.h>

namespace fe::GAPI::Pipeline
{
	using namespace Descriptors;

	struct GraphicsPipeline
	{
		GLuint OpenGLID = 0;
		GLint RootDataOffsetUniformLocation;
		Raster::Specification mRaster;

		void CreateCmd(const Resource::Shader& vertexShader, const Resource::Shader& fragmentShader, const Raster::Specification& spec)
		{
			FE_PROFILER_FUNC();

			OpenGLID = glCreateProgram();
			mRaster = spec;

			glAttachShader(OpenGLID, vertexShader.OpenGLID);
			glAttachShader(OpenGLID, fragmentShader.OpenGLID);

			GLint linking_success = 0;
			{
				FE_PROFILER_SCOPE("OpenGL Shader linking");
				glLinkProgram(OpenGLID);
				glGetProgramiv(OpenGLID, GL_LINK_STATUS, (int*)&linking_success);
			}

			if (linking_success == GL_FALSE)
			{
				GLint log_length = 0;
				glGetProgramiv(OpenGLID, GL_INFO_LOG_LENGTH, &log_length);

				Pile p;
				auto info_log = p.Allocate<GLchar>(log_length);
				glGetProgramInfoLog(OpenGLID, log_length, &log_length, info_log.Elements);

				glDeleteProgram(OpenGLID);
				OpenGLID = 0;

				FE_LOG_CORE_ERROR("{0}", info_log.Elements); // is this null terminated?
				FE_CORE_ASSERT(false, "OpenGL shader program linking failed!");
				return;
			}
			
			glDetachShader(OpenGLID, vertexShader.OpenGLID);
			glDetachShader(OpenGLID, fragmentShader.OpenGLID);

			GLint location = glGetUniformLocation(OpenGLID, "RootDataOffset");
		}
	};

	struct ComputePipeline
	{

	};
}