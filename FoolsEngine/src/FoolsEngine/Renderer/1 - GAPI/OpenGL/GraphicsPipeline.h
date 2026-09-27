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
				return GL_TRIANGLES;
			case Raster::PrimitiveType::Point:		return GL_POINTS;
			case Raster::PrimitiveType::Line:		return GL_LINES;
			case Raster::PrimitiveType::Triangle:	return GL_TRIANGLES;
			default:
				FE_CORE_ASSERT(false, "Uknown PrimitiveType");
				return GL_TRIANGLES;
			}
		}

		GLenum DepthTestTypeToGLenum(DepthStencil::DepthTestType test)
		{
			switch (test)
			{
			case DepthStencil::DepthTestType::None:
				FE_CORE_ASSERT(false, "Not specified PrimitiveType");
				return GL_GEQUAL;
			case DepthStencil::DepthTestType::Never:		return GL_NEVER;
			case DepthStencil::DepthTestType::Always:		return GL_ALWAYS;
			case DepthStencil::DepthTestType::NotEqual:		return GL_NOTEQUAL;
			case DepthStencil::DepthTestType::Less:			return GL_LESS;
			case DepthStencil::DepthTestType::LessEqual:	return GL_LEQUAL;
			case DepthStencil::DepthTestType::Equal:		return GL_EQUAL;
			case DepthStencil::DepthTestType::GreaterEqual:	return GL_GEQUAL;
			case DepthStencil::DepthTestType::Greater:		return GL_GREATER;
			default:
				FE_CORE_ASSERT(false, "Uknown DepthTestType");
				return GL_GEQUAL;
			}
		}
	}

	struct GraphicsPipeline
	{
		constexpr static ObjType Type = ObjType::GraphicsPipeline;

		GLuint mOpenGLID = 0;
		GLuint mRootDataOffsetUniformLocation;
		Raster::Specification mRaster;
		GLuint mFramebufferOpenGLID = 0;
		DepthStencil::Specification mDepthStencil;
		Blend::Specification mBlend;
		Array<GLuint, 8> mUniformBuffers = { 0,0,0,0,0,0,0,0 };
		Array<GLuint, 8> mShaderStorageBuffers = { 0,0,0,0,0,0,0,0 };
		GLuint mInidicesBuffer = 0;
		GLuint mDrawParamsBuffer = 0;

		void Init()
		{
			mOpenGLID = 0;
			mRootDataOffsetUniformLocation = -1;
			mRaster = Raster::Specification();
			mFramebufferOpenGLID = 0;
			mDepthStencil = DepthStencil::Specification();
			mBlend = Blend::Specification();
			mUniformBuffers = { 0,0,0,0,0,0,0,0 };
			mShaderStorageBuffers = { 0,0,0,0,0,0,0,0 };
			mInidicesBuffer = 0;
			mDrawParamsBuffer = 0;
		}

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
			glAttachShader(mOpenGLID, vertexShader.mOpenGLID);
			glAttachShader(mOpenGLID, fragmentShader.mOpenGLID);

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
			
			glDetachShader(mOpenGLID, vertexShader.mOpenGLID);
			glDetachShader(mOpenGLID, fragmentShader.mOpenGLID);

			mRootDataOffsetUniformLocation = glGetUniformLocation(mOpenGLID, "RootDataOffset");

			// framebuffer cleanup
			glDeleteTextures(attachment_count + depth_present, tmp_textures.Elements);

			// to do: test to see if we need a fake draw call (shader compilation may be defferred by opengl)
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
			mUniformBuffers[bindingIndex] = stream->mOpenGLBuffer;
		}

		void SetShaderStorageBuffer(const Buffer* buffer, UInt bindingIndex)
		{
			mShaderStorageBuffers[bindingIndex] = buffer->mGLID;
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
			mInidicesBuffer = stream->mOpenGLBuffer;
		}

		void SetDrawParamsBuffer(const Stream* stream)
		{
			mDrawParamsBuffer = stream->mOpenGLBuffer;
		}

		void ActivateCmd()
		{
			for (GLuint i = 0; i < mUniformBuffers.Count; i++)
			{
				glBindBufferBase(GL_UNIFORM_BUFFER, i, mUniformBuffers[i]);
			}

			for (GLuint i = 0; i < mShaderStorageBuffers.Count; i++)
			{
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, i, mShaderStorageBuffers[i]);
			}

			glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mDrawParamsBuffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mInidicesBuffer);

			if (mDepthStencil.mDepthTest)
			{
				glEnable(GL_DEPTH_TEST);
				glDepthFunc(Utils::DepthTestTypeToGLenum(mDepthStencil.mDepthTestType));
			}
			else
				glDisable(GL_DEPTH_TEST);

			if (mRaster.mFaceCullTest == Raster::FaceCullTest::Never)
			{
				glDisable(GL_CULL_FACE);
			}
			else
			{
				glEnable(GL_CULL_FACE);
				
				if (mRaster.mFaceCullTest == Raster::FaceCullTest::Back)
					glCullFace(GL_BACK);
				else if (mRaster.mFaceCullTest == Raster::FaceCullTest::Front)
					glCullFace(GL_FRONT);
				else if (mRaster.mFaceCullTest == Raster::FaceCullTest::Always)
					glCullFace(GL_FRONT_AND_BACK);
			}

			glUseProgram(mOpenGLID);

			glBindFramebuffer(GL_FRAMEBUFFER, mFramebufferOpenGLID);
		}

		void SetRootDataOffset(U32 offset)
		{
			glUniform1ui(mRootDataOffsetUniformLocation, offset);
		}

		void DrawCmd(UInt primitiveCount, UInt indicesOffset)
		{
			GLenum mode = Utils::PrimitiveTypeToGLmode(mRaster.mPrimitiveType);

			glDrawElements(mode, primitiveCount, GL_UNSIGNED_INT, (void*)indicesOffset);
		}

		void DrawIndirectCmd(UInt paramsOffset)
		{
			GLenum mode = Utils::PrimitiveTypeToGLmode(mRaster.mPrimitiveType);

			glDrawElementsIndirect(mode, GL_UNSIGNED_INT, (void*)paramsOffset);
		}

		void MultiDrawIndirectCmd(UInt paramsOffset, UInt drawCount)
		{
			GLenum mode = Utils::PrimitiveTypeToGLmode(mRaster.mPrimitiveType);

			glMultiDrawElementsIndirect(mode, GL_UNSIGNED_INT, (void*)paramsOffset, drawCount, sizeof(Data::DrawParams));
		}

		void MultiDrawIndirectCountCmd(UInt paramsOffset, UInt maxDrawCount)
		{
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