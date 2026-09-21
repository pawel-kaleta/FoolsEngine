#pragma once

#include "FoolsEngine/Foundation/Memory/DataTypes.h"
#include "FoolsEngine/Foundation/Memory/Pile.h"
#include "FoolsEngine/Renderer/1 - GAPI/Pipeline.h"
#include "FoolsEngine/Renderer/1 - GAPI/Data.h"
#include "Texture.h"
#include "Shader.h"
#include "Buffer.h"

#include <glad/gl.h>

namespace fe::GAPI::OpenGL
{
	using namespace Pipeline;

	namespace Utils
	{
		GLenum PrimitiveTypeToGLmode(Raster::PrimitiveType primitive)
		{
			// TO DO: make this a static lookup table?

			switch (primitive.Value)
			{
			case Raster::PrimitiveType::None:
				FE_CORE_ASSERT(false, "Not specified PrimitiveType");
				return GL_NONE;
			case Raster::PrimitiveType::Point:		return GL_POINTS;
			case Raster::PrimitiveType::Line:		return GL_LINES;
			case Raster::PrimitiveType::Triangle:	return GL_TRIANGLES;
			default:
				FE_CORE_ASSERT(false, "Uknown PrimitiveType");
				return GL_NONE;
			}
		}
	}

	struct GraphicsPipeline
	{
		GLuint mOpenGLID = 0;
		GLint mRootDataOffsetUniformLocation;
		Raster::Specification mRaster;
		GLuint mFramebufferOpenGLID = 0;
		DepthStencil::Specification mDepthStencil;
		Blend::Specification mBlend;
		Array<GLuint, 8> mUniformBuffers = { 0,0,0,0,0,0,0,0 };
		Array<GLuint, 8> mShaderStorageBuffers = { 0,0,0,0,0,0,0,0 };
		GLuint mInidicesBuffer = 0;
		GLuint mDrawParamsBuffer = 0;
		bool mActive = false;

		void CreateCmd(const Shader& vertexShader, const Shader& fragmentShader, const Raster::Specification& spec)
		{
			FE_PROFILER_FUNC();

			mOpenGLID = glCreateProgram();
			glCreateFramebuffers(1, &mFramebufferOpenGLID);
			mRaster = spec;

			Pile p;

			// framebuffer setup for state based compilation
			bool depth_present = spec.mDepthStencilFormat != Resource::Descriptors::TextureFormat::None;
			UInt attachment_count = spec.mColorAttachments.Count;
			auto tmp_textures = p.Allocate<GLuint>(attachment_count + depth_present);
			auto attachment_enums = p.Allocate<GLenum>(attachment_count);

			glCreateTextures(GL_TEXTURE_2D, attachment_count + depth_present, tmp_textures.Elements);

			for (UInt i = 0; i < attachment_count; i++)
			{
				GLenum internal_format = Utils::FormatToGLInternalFormat(spec.mColorAttachments[i].Format);
				glTextureStorage2D(tmp_textures[i], 1, internal_format, 4, 4);
				attachment_enums[i] = GL_COLOR_ATTACHMENT0 + i;
				glNamedFramebufferTexture(mFramebufferOpenGLID, attachment_enums[i], GL_RENDERBUFFER, tmp_textures[i]);
			}
			glNamedFramebufferDrawBuffers(mFramebufferOpenGLID, (GLsizei)attachment_count, attachment_enums.Elements);

			if (depth_present)
			{
				GLenum internal_format = Utils::FormatToGLInternalFormat(spec.mDepthStencilFormat);
				glTextureStorage2D(tmp_textures[attachment_count], 1, internal_format, 4, 4);
				glNamedFramebufferTexture(mFramebufferOpenGLID, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, tmp_textures[attachment_count]);
			}

			auto status = glCheckNamedFramebufferStatus(mFramebufferOpenGLID, GL_FRAMEBUFFER);
			if (status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)
				FE_LOG_CORE_ERROR("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
			if (status == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT)
				FE_LOG_CORE_ERROR("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
			if (status == GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER)
				FE_LOG_CORE_ERROR("GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER");
			if (status == GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER)
				FE_LOG_CORE_ERROR("GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER");
			if (status == GL_FRAMEBUFFER_UNSUPPORTED)
				FE_LOG_CORE_ERROR("GL_FRAMEBUFFER_UNSUPPORTED");
			

			// facecull setup for state based compilation
			switch (spec.mFaceCullTest.Value)
			{
			case Raster::FaceCullTest::None:
				FE_CORE_ASSERT(false, "Unset facecull test");
				glDisable(GL_CULL_FACE);
				break;
			case Raster::FaceCullTest::Never:
				glDisable(GL_CULL_FACE);
				break;
			case Raster::FaceCullTest::Back:
				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);
				break;
			case Raster::FaceCullTest::Front:
				glEnable(GL_CULL_FACE);
				glCullFace(GL_FRONT);
				break;
			case Raster::FaceCullTest::Always:
				glEnable(GL_CULL_FACE);
				glCullFace(GL_FRONT_AND_BACK);
				break;
			}


			// compilation
			glAttachShader(mOpenGLID, vertexShader.OpenGLID);
			glAttachShader(mOpenGLID, fragmentShader.OpenGLID);

			GLint linking_success = 0;
			{
				FE_PROFILER_SCOPE("OpenGL Shader linking");
				glBindFramebuffer(GL_FRAMEBUFFER, mFramebufferOpenGLID);
				glLinkProgram(mOpenGLID);
				glGetProgramiv(mOpenGLID, GL_LINK_STATUS, (int*)&linking_success);
			}

			if (linking_success == GL_FALSE)
			{
				GLint log_length = 0;
				glGetProgramiv(mOpenGLID, GL_INFO_LOG_LENGTH, &log_length);

				auto info_log = p.Allocate<GLchar>(log_length);
				glGetProgramInfoLog(mOpenGLID, log_length, &log_length, info_log.Elements);

				glDeleteProgram(mOpenGLID);
				mOpenGLID = 0;

				FE_LOG_CORE_ERROR("{0}", info_log.Elements); // is this null terminated?
				FE_CORE_ASSERT(false, "OpenGL shader program linking failed!");
				return;
			}
			
			glDetachShader(mOpenGLID, vertexShader.OpenGLID);
			glDetachShader(mOpenGLID, fragmentShader.OpenGLID);

			mRootDataOffsetUniformLocation = glGetUniformLocation(mOpenGLID, "RootDataOffset");

			// framebuffer cleanup
			glDeleteTextures(attachment_count + depth_present, tmp_textures.Elements);
		}

		void SetColorAttachment(const Texture& texture, UInt index)
		{
			FE_CORE_ASSERT(texture.Spec.mFormat == mRaster.mColorAttachments[index].Format, "Wrong texture format!");		
			glNamedFramebufferTexture(mFramebufferOpenGLID, GL_COLOR_ATTACHMENT0 + index, texture.OpenGLID, 1);
		}

		void SetDepthStencilAttachment(const Texture& texture)
		{
			FE_CORE_ASSERT(texture.Spec.mFormat == mRaster.mDepthStencilFormat, "Wrong texture format!");
			glNamedFramebufferTexture(mFramebufferOpenGLID, GL_DEPTH_STENCIL_ATTACHMENT, texture.OpenGLID, 1);
		}

		void SetDepthStencilSpec(const DepthStencil::Specification& depthStencil)
		{
			mDepthStencil = depthStencil;
		}

		void SetBlendSpec(const Blend::Specification& blend)
		{
			mBlend = blend;
		}

		void SetUniformBuffer(const Stream* stream, UInt bindingIndex)
		{
			mUniformBuffers[bindingIndex] = stream->OpenGLBuffer;
		}

		void SetIndicesBuffer(const Buffer* buffer)
		{
			mInidicesBuffer = buffer->mGLID;
		}

		void SetDrawParamsBuffer(const Buffer* buffer)
		{
			mDrawParamsBuffer = buffer->mGLID;
		}

		void SetIndicesBuffer(const Stream* stream)
		{
			mInidicesBuffer = stream->OpenGLBuffer;
		}

		void SetDrawParamsBuffer(const Stream* stream)
		{
			mDrawParamsBuffer = stream->OpenGLBuffer;
		}

		void ActivateCmd()
		{

		}

		void DrawCmd(UInt primitiveCount, UInt indicesOffset)
		{
			FE_CORE_ASSERT(mActive, "Pipeline is not active");

			GLenum mode = Utils::PrimitiveTypeToGLmode(mRaster.mPrimitiveType);

			glDrawElements(mode, primitiveCount, GL_UNSIGNED_INT, (void*)indicesOffset);
		}

		void DrawIndirectCmd(UInt paramsOffset)
		{
			FE_CORE_ASSERT(mActive, "Pipeline is not active");

			GLenum mode = Utils::PrimitiveTypeToGLmode(mRaster.mPrimitiveType);

			glDrawElementsIndirect(mode, GL_UNSIGNED_INT, (void*)paramsOffset);
		}

		void MultiDrawIndirectCmd(UInt paramsOffset, UInt drawCount)
		{
			FE_CORE_ASSERT(mActive, "Pipeline is not active");

			GLenum mode = Utils::PrimitiveTypeToGLmode(mRaster.mPrimitiveType);

			glMultiDrawElementsIndirect(mode, GL_UNSIGNED_INT, (void*)paramsOffset, drawCount, sizeof(Data::DrawParams));
		}

		void MultiDrawIndirectCountCmd(UInt paramsOffset, UInt maxDrawCount)
		{
			FE_CORE_ASSERT(mActive, "Pipeline is not active");

			GLenum mode = Utils::PrimitiveTypeToGLmode(mRaster.mPrimitiveType);

			glMultiDrawElementsIndirectCount(mode, GL_UNSIGNED_INT, (void*)(paramsOffset + 4), paramsOffset, maxDrawCount, sizeof(Data::DrawParams));
		}
		
		void DestroyCmd()
		{
			glDeleteFramebuffers(1, &mFramebufferOpenGLID);
			glDeleteProgram(mOpenGLID);
		}
	};
}