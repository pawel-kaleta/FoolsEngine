#include "FE_pch.h"
#include "OpenGLFramebuffer.h"

#include <glad\glad.h>

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
		m_Specification.Width = width;
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

	const uint32_t fe::OpenGLFramebuffer::GetColorAttachmentID() const
	{
		return m_ColorAttachment;
	}

	const FramebufferData::Specification& fe::OpenGLFramebuffer::GetSpecification() const
	{
		return m_Specification;
	}

	void OpenGLFramebuffer::Create()
	{
		glCreateFramebuffers(1, &m_ID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_ID);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachment);
		glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Specification.Width, m_Specification.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachment);
		glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);

		FE_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is not complete.");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::Delete()
	{
		glDeleteFramebuffers(1, &m_ID);
		glDeleteTextures(1, &m_ColorAttachment);
		glDeleteTextures(1, &m_DepthAttachment);
	}
}