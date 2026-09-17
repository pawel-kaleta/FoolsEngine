#pragma once

#include "FoolsEngine/Foundation/Memory/DataTypes.h"
#include "FoolsEngine/Foundation/Memory/Pile.h"
#include "Shader.h"
#include "Descriptors.h"
#include "Texture.h"

#include <glad/glad.h>

namespace fe::GAPI::Pipeline
{
	using namespace Descriptors;

	struct GraphicsPipeline
	{
		GLuint OpenGLID = 0;
		GLint RootDataOffsetUniformLocation;
		Raster::Specification mRaster;
		GLuint FramebufferOpenGLID = 0;
		ArrayArena<Resource::Texture*, 6> mColorAttachments;
		Resource::Texture* mDepthStencilAttachment = nullptr;
		DepthStencil::Specification mDepthStencil;
		Blend::Specification mBlend;
		bool mDepthStencilChanged = false;
		bool mBlendChanged = false;

		void CreateCmd(const Resource::Shader& vertexShader, const Resource::Shader& fragmentShader, const Raster::Specification& spec)
		{
			FE_PROFILER_FUNC();

			OpenGLID = glCreateProgram();
			glCreateFramebuffers(1, &FramebufferOpenGLID);
			mRaster = spec;

			Pile p;

			// creating fake textures

			// USE RENDERBUFFERS INSTEAD OF TEXTURES!!!
			{
				UInt textures_count = spec.mColorAttachments.Count;
				auto tmp_textures = p.Allocate<Resource::Texture>(textures_count);
				auto enums = p.Allocate<GLenum>(textures_count);

				for (UInt i = 0; i < textures_count; i++)
				{
					mColorAttachments.Append((Resource::Texture*)nullptr);

					Texture::Specification tmp_spec =
					{
						.mType = Texture::Type::Texture2D,
						.mFormat = spec.mColorAttachments[i].Format,
						.mDimentions = { 4, 4, 0 }
					};

					tmp_textures[i].Make(tmp_spec);
					tmp_textures[i].Allocate();

					glNamedFramebufferTexture(FramebufferOpenGLID, GL_COLOR_ATTACHMENT0 + i, tmp_textures[i].OpenGLID, 0);
					enums[i] = GL_COLOR_ATTACHMENT0 + i;
				}

				glNamedFramebufferDrawBuffers(OpenGLID, (GLsizei)textures_count, enums.Elements);
				

				
				
				if (spec.mDepthStencilFormat == Texture::Format::DEPTH24STENCIL8)
				{
					Texture::Specification tmp_spec =
					{
						.mType = Texture::Type::Texture2D,
						.mFormat = spec.mDepthStencilFormat,
						.mDimentions = { 4, 4, 0 }
					};


				}
			}

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

		void SetColorAttachment(Resource::Texture* texture, UInt index)
		{
			FE_CORE_ASSERT(texture->Spec.mFormat == mRaster.mColorAttachments[index].Format, "Texture does not have format required by this pipeline in this location");
			mColorAttachments[index] = texture;
		}

		void SetDepthStencil(const DepthStencil::Specification& depthStencil)
		{
			mDepthStencil = depthStencil;
			mDepthStencilChanged = true;
		}
	};
}