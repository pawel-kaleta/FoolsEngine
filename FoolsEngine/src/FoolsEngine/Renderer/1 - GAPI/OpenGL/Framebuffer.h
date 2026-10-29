#pragma once

#include "FoolsEngine/Foundation/Containers/Arena.h"
#include "FoolsEngine/Foundation/DataTypes.h"
#include "FoolsEngine/Foundation/Context/Pile.h"
#include "FoolsEngine/Foundation/Utils/TypeSetup.h"

#include "Texture.h"

#include <glad/gl.h>

namespace fe::Render::GAPI::OpenGL
{
	struct Framebuffer
	{
		Descriptors::FramebufferSpec mSpec;
		ArrayArena<GLuint, 6>	mColorAttachments;
		GLuint					mDepthStencilAttachment;
		glm::vec<2, U32>		mDimentions;
		GLuint					mOpenGLID;

		FE_TYPE_SETUP(Framebuffer);

		Framebuffer& Init()
		{
			mOpenGLID = 0;
			mSpec.Init();
			mColorAttachments.Init();
			mDepthStencilAttachment = 0;
			mDimentions = { 1, 1 };
			return *this;
		}

		void Create(const Descriptors::FramebufferSpec& specification, UInt width, UInt height)
		{
			glCreateFramebuffers(1, &mOpenGLID);

			mSpec = specification;

			Pile p;

			mColorAttachments.Count = specification.mColorFormats.Count;
			auto attachment_enums = p.Allocate<GLenum>(specification.mColorFormats.Count);

			glCreateTextures(GL_TEXTURE_2D, specification.mColorFormats.Count, mColorAttachments.begin());

			for (UInt i = 0; i < specification.mColorFormats.Count; i++)
			{
				GLenum internal_format = Utils::FormatToGLInternalFormat(specification.mColorFormats[i]);
				glTextureStorage2D(mColorAttachments[i], 1, internal_format, width, height);
				attachment_enums[i] = GL_COLOR_ATTACHMENT0 + i;
				glNamedFramebufferTexture(mOpenGLID, attachment_enums[i], mColorAttachments[i], 0);
			}
			glNamedFramebufferDrawBuffers(mOpenGLID, (GLsizei)specification.mColorFormats.Count, attachment_enums.Elements);

			bool depth_present = specification.mDepthStencilFormat != Descriptors::TextureFormat::None;
			if (depth_present)
			{
				glCreateTextures(GL_TEXTURE_2D, 1, &mDepthStencilAttachment);
				GLenum internal_format = Utils::FormatToGLInternalFormat(specification.mDepthStencilFormat);
				glTextureStorage2D(mDepthStencilAttachment, 1, internal_format, width, height);
				glNamedFramebufferTexture(mOpenGLID, GL_DEPTH_STENCIL_ATTACHMENT, 1, 0);
			}

			auto status = glCheckNamedFramebufferStatus(mOpenGLID, GL_FRAMEBUFFER);
			if (status != GL_FRAMEBUFFER_COMPLETE)
			{
				if (status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)
					Context::Log::Core.Error("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"_FE);
				if (status == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT)
					Context::Log::Core.Error("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"_FE);
				if (status == GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER)
					Context::Log::Core.Error("GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"_FE);
				if (status == GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER)
					Context::Log::Core.Error("GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER"_FE);
				if (status == GL_FRAMEBUFFER_UNSUPPORTED)
					Context::Log::Core.Error("GL_FRAMEBUFFER_UNSUPPORTED"_FE);
			}
		}

		void Resize(UInt width, UInt height)
		{
			glDeleteTextures(mSpec.mColorFormats.Count, mColorAttachments.begin());
			glCreateTextures(GL_TEXTURE_2D, mSpec.mColorFormats.Count, mColorAttachments.begin());

			for (UInt i = 0; i < mSpec.mColorFormats.Count; i++)
			{
				GLenum internal_format = Utils::FormatToGLInternalFormat(mSpec.mColorFormats[i]);
				glTextureStorage2D(mColorAttachments[i], 1, internal_format, width, height);
				glNamedFramebufferTexture(mOpenGLID, GL_COLOR_ATTACHMENT0 + i, mColorAttachments[i], 0);
			}

			bool depth_present = mSpec.mDepthStencilFormat != Descriptors::TextureFormat::None;
			if (depth_present)
			{
				glDeleteTextures(1, &mDepthStencilAttachment);
				glCreateTextures(GL_TEXTURE_2D, 1, &mDepthStencilAttachment);
				GLenum internal_format = Utils::FormatToGLInternalFormat(mSpec.mDepthStencilFormat);
				glTextureStorage2D(mDepthStencilAttachment, 1, internal_format, width, height);
				glNamedFramebufferTexture(mOpenGLID, GL_DEPTH_STENCIL_ATTACHMENT, 1, 0);
			}

			auto status = glCheckNamedFramebufferStatus(mOpenGLID, GL_FRAMEBUFFER);
			if (status != GL_FRAMEBUFFER_COMPLETE)
			{
				if (status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)
					Context::Log::Core.Error("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"_FE);
				if (status == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT)
					Context::Log::Core.Error("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"_FE);
				if (status == GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER)
					Context::Log::Core.Error("GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"_FE);
				if (status == GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER)
					Context::Log::Core.Error("GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER"_FE);
				if (status == GL_FRAMEBUFFER_UNSUPPORTED)
					Context::Log::Core.Error("GL_FRAMEBUFFER_UNSUPPORTED"_FE);
			}
		}

		void Destroy()
		{
			glDeleteTextures(mSpec.mColorFormats.Count, mColorAttachments.begin());

			bool depth_present = mSpec.mDepthStencilFormat != Descriptors::TextureFormat::None;
			if (depth_present) glDeleteTextures(1, &mDepthStencilAttachment);

			Init();
		}
	};
}