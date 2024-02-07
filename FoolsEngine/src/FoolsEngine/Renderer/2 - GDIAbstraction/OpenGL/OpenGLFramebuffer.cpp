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

	void fe::OpenGLFramebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
		glViewport(0, 0, m_Specification.Width, m_Specification.Height);
	}

	void fe::OpenGLFramebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	GLenum OpenGLFramebuffer::FormatToGLenum(TextureData::Components format)
	{
		switch (format)
		{
		case TextureData::Components::None:
			FE_CORE_ASSERT(false, "Not specified format of attachment");
			return GL_NONE;
		case TextureData::Components::RGB:  return GL_RGB;
		case TextureData::Components::RGBA: return GL_RGBA;
		case TextureData::Components::DepthStencil: return GL_DEPTH_STENCIL_ATTACHMENT;
		default:
			FE_CORE_ASSERT(false, "Uknown format of attachment");
			return 0;
		}
	}

	GLenum OpenGLFramebuffer::DataFormatToGLenum(TextureData::Format dataFormat)
	{
		switch (dataFormat)
		{
		case TextureData::Format::None:
			FE_CORE_ASSERT(false, "Not specified data format of attachment");
			return GL_NONE;
		case TextureData::Format::RGB8:  return GL_RGB8;
		case TextureData::Format::RGBA8: return GL_RGBA8;
		case TextureData::Format::DEPTH24STENCIL8: return GL_DEPTH24_STENCIL8;
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
				GLenum format     =     FormatToGLenum(m_Specification.ColorAttachments[i].Components);
				GLenum dataFormat = DataFormatToGLenum(m_Specification.ColorAttachments[i].Format);

				glBindTexture(target, m_ColorAttachments[i]);

				if (multisampled)
				{
					glTexImage2DMultisample(target, m_Specification.Samples, format, m_Specification.Width, m_Specification.Height, GL_FALSE);
				}
				else
				{
					glTexImage2D(target, 0, dataFormat, m_Specification.Width, m_Specification.Height, 0, format, GL_UNSIGNED_BYTE, nullptr);

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
				GLenum dataFormat = DataFormatToGLenum(m_Specification.DepthStencilAttachment);

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