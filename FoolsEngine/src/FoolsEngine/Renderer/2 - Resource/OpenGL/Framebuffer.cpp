#include "FE_pch.h"

#include "Utils.h"

#include "FoolsEngine\Renderer\2 - Resource\Framebuffer.h"
#include "FoolsEngine\Renderer\1 - Description\Library.h"

#include <glm\gtc\type_ptr.hpp>

namespace fe::Resource
{
	using namespace Description::Texture;

	void Framebuffer_OpenGL::Create()
	{
		FE_PROFILER_FUNC();

		const auto& spec = Description::Library::Get().FramebufferSpecs[SpecificationID];

		glCreateFramebuffers(1, &FramebufferOpenGLID);
		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferOpenGLID);

		bool multisampled = spec.Samples > 1;

		if (spec.ColorAttachments.size())
		{
			ColorAttachmentOpenGLIDs.resize(spec.ColorAttachments.size());

			if (multisampled)
			{
				glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, (GLsizei)ColorAttachmentOpenGLIDs.size(), ColorAttachmentOpenGLIDs.data());

				for (int i = 0; i < ColorAttachmentOpenGLIDs.size(); ++i)
				{
					GLenum internalFormat = Utils::FormatToGLInternalFormat(spec.ColorAttachments[i].Format);
					glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, ColorAttachmentOpenGLIDs[i]);
					glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, spec.Samples, internalFormat, Width, Height, GL_FALSE);
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D_MULTISAMPLE, ColorAttachmentOpenGLIDs[i], 0);
				}
			}
			else
			{
				for (int i = 0; i < ColorAttachmentOpenGLIDs.size(); ++i)
				{
					GLenum internalFormat = Utils::FormatToGLInternalFormat(spec.ColorAttachments[i].Format);
					glBindTexture(GL_TEXTURE_2D, ColorAttachmentOpenGLIDs[i]);
					glTexStorage2D(GL_TEXTURE_2D, 1, internalFormat, Width, Height);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, ColorAttachmentOpenGLIDs[i], 0);
				}
			}
		}

		if (spec.DepthStencilFormat != Description::Texture::Format::None)
		{
			GLenum dataFormat = Utils::FormatToGLInternalFormat(spec.DepthStencilFormat);

			if (multisampled)
			{
				glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &DepthStencilAttachmentOpenGLID);
				glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, DepthStencilAttachmentOpenGLID);
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, spec.Samples, dataFormat, Width, Height, GL_FALSE);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, DepthStencilAttachmentOpenGLID, 0);
			}
			else
			{
				glCreateTextures(GL_TEXTURE_2D, 1, &DepthStencilAttachmentOpenGLID);
				glBindTexture(GL_TEXTURE_2D, DepthStencilAttachmentOpenGLID);
				glTexStorage2D(GL_TEXTURE_2D, 1, dataFormat, Width, Height);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, DepthStencilAttachmentOpenGLID, 0);
			}
		}

		if (ColorAttachmentOpenGLIDs.size() > 1)
		{
			Scratchpad sp;

			std::pmr::vector<GLenum> buffers(ColorAttachmentOpenGLIDs.size(), &sp);
			for (int i = 0; i < ColorAttachmentOpenGLIDs.size(); ++i)
			{
				buffers[i] = GL_COLOR_ATTACHMENT0 + i;
			}
			glDrawBuffers((GLsizei)ColorAttachmentOpenGLIDs.size(), buffers.data());
		}
		else if (ColorAttachmentOpenGLIDs.empty())
		{
			glDrawBuffer(GL_NONE);
		}

		auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
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

		FE_CORE_ASSERT(status == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is not complete.");
	}

	void Framebuffer_OpenGL::Resize(uint32_t width, uint32_t height)
	{
		FE_PROFILER_FUNC();

		const auto& spec = Description::Library::Get().FramebufferSpecs[SpecificationID];

		Width = width;
		Height = height;

		bool multisampled = spec.Samples > 1;

		if (spec.ColorAttachments.size())
		{
			ColorAttachmentOpenGLIDs.resize(spec.ColorAttachments.size());

			if (multisampled)
			{
				for (int i = 0; i < ColorAttachmentOpenGLIDs.size(); ++i)
				{
					GLenum internalFormat = Utils::FormatToGLInternalFormat(spec.ColorAttachments[i].Format);
					glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, ColorAttachmentOpenGLIDs[i]);
					glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, spec.Samples, internalFormat, Width, Height, GL_FALSE);
				}
			}
			else
			{
				for (int i = 0; i < ColorAttachmentOpenGLIDs.size(); ++i)
				{
					GLenum internalFormat = Utils::FormatToGLInternalFormat(spec.ColorAttachments[i].Format);
					glBindTexture(GL_TEXTURE_2D, ColorAttachmentOpenGLIDs[i]);
					glTexStorage2D(GL_TEXTURE_2D, 1, internalFormat, Width, Height);
				}
			}
		}

		if (spec.DepthStencilFormat != Description::Texture::Format::None)
		{
			GLenum dataFormat = Utils::FormatToGLInternalFormat(spec.DepthStencilFormat);

			if (multisampled)
			{
				glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, DepthStencilAttachmentOpenGLID);
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, spec.Samples, dataFormat, Width, Height, GL_FALSE);
			}
			else
			{
				glBindTexture(GL_TEXTURE_2D, DepthStencilAttachmentOpenGLID);
				glTexStorage2D(GL_TEXTURE_2D, 1, dataFormat, Width, Height);
			}
		}
	}

	void Framebuffer_OpenGL::ClearAttachment(uint32_t attachmentIndex, uint32_t value)
	{
		FE_PROFILER_FUNC();

		FE_CORE_ASSERT(attachmentIndex < ColorAttachmentOpenGLIDs.size(), "Framebuffer attachment index out of bounds");

		const auto& spec = Description::Library::Get().FramebufferSpecs[SpecificationID];

		auto& format = spec.ColorAttachments[attachmentIndex].Format;
		glClearTexImage(ColorAttachmentOpenGLIDs[attachmentIndex], 0, Utils::FormatToGLFormat(format), GL_UNSIGNED_INT, &value);
	}

	void Framebuffer_OpenGL::ClearAttachment(uint32_t attachmentIndex, float value)
	{
		FE_PROFILER_FUNC();

		FE_CORE_ASSERT(attachmentIndex < ColorAttachmentOpenGLIDs.size(), "Framebuffer attachment index out of bounds");

		const auto& spec = Description::Library::Get().FramebufferSpecs[SpecificationID];

		auto& format = spec.ColorAttachments[attachmentIndex].Format;
		glClearTexImage(ColorAttachmentOpenGLIDs[attachmentIndex], 0, Utils::FormatToGLFormat(format), GL_FLOAT, &value);
	}

	void Framebuffer_OpenGL::ReadPixel(uint32_t attachmentIndex, int x, int y, void* destination) const
	{
		FE_PROFILER_FUNC();

		FE_CORE_ASSERT(attachmentIndex < ColorAttachmentOpenGLIDs.size(), "Framebuffer attachment index out of bounds");

		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);

		const auto& spec = Description::Library::Get().FramebufferSpecs[SpecificationID];
		auto& format = spec.ColorAttachments[attachmentIndex].Format;
		GLenum glFormat = Utils::FormatToGLFormat(format);
		GLenum glType = Utils::FormatToGLType(format);
		glReadPixels(x, y, 1, 1, glFormat, glType, destination);
	}

	void Framebuffer_OpenGL::Destroy()
	{
		FE_PROFILER_FUNC();

		glDeleteFramebuffers(1, &FramebufferOpenGLID);
		if (ColorAttachmentOpenGLIDs.size())
			glDeleteTextures((GLsizei)ColorAttachmentOpenGLIDs.size(), ColorAttachmentOpenGLIDs.data());
		if (DepthStencilAttachmentOpenGLID)
			glDeleteTextures(1, &DepthStencilAttachmentOpenGLID);

		ColorAttachmentOpenGLIDs.clear();
	}
}