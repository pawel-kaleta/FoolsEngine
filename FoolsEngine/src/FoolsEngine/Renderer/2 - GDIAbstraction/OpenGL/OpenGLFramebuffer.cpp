#include "FE_pch.h"
#include "OpenGLFramebuffer.h"

namespace fe
{
	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferData::Specification& spec)
		: m_Specification(spec)
	{
		Create();
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		Delete();
	}

	void OpenGLFramebuffer::Recreate(const FramebufferData::Specification& spec)
	{
		m_Specification = spec;
		Recreate();
	}

	void OpenGLFramebuffer::Recreate()
	{
		Delete();
		Create();
	}

	void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height)
	{
		m_Specification.Width  = width;
		m_Specification.Height = height;
		Recreate();
	}

	void OpenGLFramebuffer::ClearAttachment(uint32_t attachmentIndex, uint32_t value)
	{
		FE_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size(), "Framebuffer attachment index out of bounds");

		auto& format = m_Specification.ColorAttachments[attachmentIndex].Format;
		glClearTexImage(m_ColorAttachments[attachmentIndex], 0, FormatToGLformat(format), GL_UNSIGNED_INT, &value);
	}

	void OpenGLFramebuffer::ClearAttachment(uint32_t attachmentIndex, float value)
	{
		FE_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size(), "Framebuffer attachment index out of bounds");

		auto& format = m_Specification.ColorAttachments[attachmentIndex].Format;
		glClearTexImage(m_ColorAttachments[attachmentIndex], 0, FormatToGLformat(format), GL_FLOAT, &value);
	}

	void fe::OpenGLFramebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
		glViewport(0, 0, m_Specification.Width, m_Specification.Height);
	}

	void fe::OpenGLFramebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y, void* saveLocation)
	{
		FE_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size(), "Framebuffer attachment index out of bounds");

		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
		auto& format = m_Specification.ColorAttachments[attachmentIndex].Format;
		GLenum glFormat = FormatToGLformat(format);
		GLenum glType = FormatToGLtype(format);
		glReadPixels(x, y, 1, 1, glFormat, glType, saveLocation);
	
	}

	GLenum OpenGLFramebuffer::FormatToGLinternalFormat(TextureData::Format format)
	{
		switch (format)
		{
		case TextureData::Format::None:
			FE_CORE_ASSERT(false, "Not specified data format of attachment");
			return GL_NONE;
		case TextureData::Format::R_8:				return GL_R8;
		case TextureData::Format::RG_8:				return GL_RG8;
		case TextureData::Format::RGB_8:			return GL_RGB8;
		case TextureData::Format::RGBA_8:			return GL_RGBA8;
		case TextureData::Format::R_UINT_32:		return GL_R32UI;
		case TextureData::Format::DEPTH24STENCIL8:	return GL_DEPTH24_STENCIL8;
		default:
			FE_CORE_ASSERT(false, "Uknown data format of attachment");
			return GL_NONE;
		}
	}

	GLenum OpenGLFramebuffer::FormatToGLtype(TextureData::Format format)
	{
		switch (format)
		{
		case TextureData::Format::None:
			FE_CORE_ASSERT(false, "Not specified data format of attachment");
			return GL_NONE;
		case TextureData::Format::R_8:				return GL_UNSIGNED_BYTE;
		case TextureData::Format::RG_8:				return GL_UNSIGNED_BYTE;
		case TextureData::Format::RGB_8:			return GL_UNSIGNED_BYTE;
		case TextureData::Format::RGBA_8:			return GL_UNSIGNED_BYTE;
		case TextureData::Format::R_UINT_32:		return GL_UNSIGNED_INT;
		case TextureData::Format::DEPTH24STENCIL8:	return GL_DEPTH24_STENCIL8;
		default:
			FE_CORE_ASSERT(false, "Uknown data format of attachment");
			return GL_NONE;
		}
	}

	GLenum OpenGLFramebuffer::FormatToGLformat(TextureData::Format format)
	{
		switch (format)
		{
		case TextureData::Format::None:
			FE_CORE_ASSERT(false, "Not specified data format of attachment");
			return GL_NONE;
		case TextureData::Format::R_8:				return GL_RED;
		case TextureData::Format::RG_8:				return GL_RG;
		case TextureData::Format::RGB_8:			return GL_RGB;
		case TextureData::Format::RGBA_8:			return GL_RGBA;
		case TextureData::Format::R_UINT_32:		return GL_RED_INTEGER;
		case TextureData::Format::DEPTH24STENCIL8:	return GL_DEPTH24_STENCIL8;
		default:
			FE_CORE_ASSERT(false, "Uknown data format of attachment");
			return GL_NONE;
		}
	}

	void OpenGLFramebuffer::Create()
	{
		glCreateFramebuffers(1, &m_ID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_ID);

		bool multisampled = m_Specification.Samples > 1;
		GLenum target = multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

		if (m_Specification.ColorAttachments.size())
		{
			m_ColorAttachments.resize(m_Specification.ColorAttachments.size());
			glCreateTextures(target, (GLsizei)m_ColorAttachments.size(), m_ColorAttachments.data());

			for (int i = 0; i < m_ColorAttachments.size(); ++i)
			{
				//GLenum format         = ComponentsToGLformat(m_Specification.ColorAttachments[i].Components);
				GLenum internalFormat = FormatToGLinternalFormat(m_Specification.ColorAttachments[i].Format);

				glBindTexture(target, m_ColorAttachments[i]);

				if (multisampled)
				{
					glTexImage2DMultisample(target, m_Specification.Samples, internalFormat, m_Specification.Width, m_Specification.Height, GL_FALSE);
				}
				else
				{
					glTexStorage2D(target, 1, internalFormat, m_Specification.Width, m_Specification.Height);

					glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

					glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
					glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				}

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, target, m_ColorAttachments[i], 0);
			}

			if (m_Specification.DepthStencilAttachment != TextureData::Format::None)
			{
				GLenum dataFormat = FormatToGLinternalFormat(m_Specification.DepthStencilAttachment);

				glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthStencilAttachment);
				glBindTexture(GL_TEXTURE_2D, m_DepthStencilAttachment);

				if (multisampled)
				{
					glTexImage2DMultisample(target, m_Specification.Samples, dataFormat, m_Specification.Width, m_Specification.Height, GL_FALSE);
				}
				else
				{
					glTexStorage2D(target, 1, dataFormat, m_Specification.Width, m_Specification.Height);

					glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

					glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
					glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				}

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, target, m_DepthStencilAttachment, 0);
			}
		}
		
		if (m_ColorAttachments.size() > 1)
		{
			std::vector<GLenum> buffers(m_ColorAttachments.size());
			for (int i = 0; i < m_ColorAttachments.size(); ++i)
			{
				buffers[i] = GL_COLOR_ATTACHMENT0 + i;
			}
			glDrawBuffers((GLsizei)m_ColorAttachments.size(), buffers.data());
		}
		else if (m_ColorAttachments.empty())
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

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::Delete()
	{
		glDeleteFramebuffers(1, &m_ID);
		if (m_ColorAttachments.size())
			glDeleteTextures((GLsizei)m_ColorAttachments.size(), m_ColorAttachments.data());
		if (m_DepthStencilAttachment)
			glDeleteTextures(1, &m_DepthStencilAttachment);

		m_ColorAttachments.clear();
	}
}