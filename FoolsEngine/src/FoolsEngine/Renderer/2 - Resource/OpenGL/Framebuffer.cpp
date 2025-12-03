#include "FE_pch.h"

#include "FoolsEngine\Renderer\2 - Resource\Framebuffer.h"
#include "FoolsEngine\Renderer\1 - Description\Library.h"

#include <glm\gtc\type_ptr.hpp>

namespace fe::Resource
{
	using namespace Description::Texture;

	GLenum FormatToGLinternalFormat(Format format)
	{
		// TO DO: there is a copy of this in Resource > OpenGL > Texture.cpp

		switch (format.Value)
		{
		case Format::None:
			FE_CORE_ASSERT(false, "Not specified data format of attachment");
			return GL_NONE;
		case Format::R_8:				return GL_R8;
		case Format::RG_8:				return GL_RG8;
		case Format::RGB_8:				return GL_RGB8;
		case Format::RGBA_8:			return GL_RGBA8;
		case Format::R_UINT_32:			return GL_R32UI;
		case Format::DEPTH24STENCIL8:	return GL_DEPTH24_STENCIL8;
		default:
			FE_CORE_ASSERT(false, "Uknown data format of attachment");
			return GL_NONE;
		}
	}

	void Framebuffer_OpenGL::Create()
	{
		const auto& spec = Description::Library::Get().FramebufferSpecs[m_SpecificationID];

		glCreateFramebuffers(1, &m_OpenGLID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_OpenGLID);

		bool multisampled = spec.Samples > 1;

		if (spec.ColorAttachments.size())
		{
			m_ColorAttachments.resize(spec.ColorAttachments.size());

			if (multisampled)
			{
				glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, (GLsizei)m_ColorAttachments.size(), m_ColorAttachments.data());

				for (int i = 0; i < m_ColorAttachments.size(); ++i)
				{
					GLenum internalFormat = FormatToGLinternalFormat(spec.ColorAttachments[i].Format);
					glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_ColorAttachments[i]);
					glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, spec.Samples, internalFormat, m_Width, m_Height, GL_FALSE);
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D_MULTISAMPLE, m_ColorAttachments[i], 0);
				}
			}
			else
			{
				for (int i = 0; i < m_ColorAttachments.size(); ++i)
				{
					GLenum internalFormat = FormatToGLinternalFormat(spec.ColorAttachments[i].Format);
					glBindTexture(GL_TEXTURE_2D, m_ColorAttachments[i]);
					glTexStorage2D(GL_TEXTURE_2D, 1, internalFormat, m_Width, m_Height);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_ColorAttachments[i], 0);
				}
			}
		}

		if (spec.DepthStencilFormat != Description::Texture::Format::None)
		{
			GLenum dataFormat = FormatToGLinternalFormat(spec.DepthStencilFormat);

			if (multisampled)
			{
				glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &m_DepthStencilAttachment);
				glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_DepthStencilAttachment);
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, spec.Samples, dataFormat, m_Width, m_Height, GL_FALSE);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, m_DepthStencilAttachment, 0);
			}
			else
			{
				glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthStencilAttachment);
				glBindTexture(GL_TEXTURE_2D, m_DepthStencilAttachment);
				glTexStorage2D(GL_TEXTURE_2D, 1, dataFormat, m_Width, m_Height);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthStencilAttachment, 0);
			}
		}

		if (m_ColorAttachments.size() > 1)
		{
			Scratchpad sp;

			std::pmr::vector<GLenum> buffers(m_ColorAttachments.size(), &sp);
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

	void Framebuffer_OpenGL::Resize(uint32_t width, uint32_t height)
	{
		const auto& spec = Description::Library::Get().FramebufferSpecs[m_SpecificationID];

		m_Width = width;
		m_Height = height;

		bool multisampled = spec.Samples > 1;

		if (spec.ColorAttachments.size())
		{
			m_ColorAttachments.resize(spec.ColorAttachments.size());

			if (multisampled)
			{
				for (int i = 0; i < m_ColorAttachments.size(); ++i)
				{
					GLenum internalFormat = FormatToGLinternalFormat(spec.ColorAttachments[i].Format);
					glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_ColorAttachments[i]);
					glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, spec.Samples, internalFormat, m_Width, m_Height, GL_FALSE);
				}
			}
			else
			{
				for (int i = 0; i < m_ColorAttachments.size(); ++i)
				{
					GLenum internalFormat = FormatToGLinternalFormat(spec.ColorAttachments[i].Format);
					glBindTexture(GL_TEXTURE_2D, m_ColorAttachments[i]);
					glTexStorage2D(GL_TEXTURE_2D, 1, internalFormat, m_Width, m_Height);
				}
			}
		}

		if (spec.DepthStencilFormat != Description::Texture::Format::None)
		{
			GLenum dataFormat = FormatToGLinternalFormat(spec.DepthStencilFormat);

			if (multisampled)
			{
				glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_DepthStencilAttachment);
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, spec.Samples, dataFormat, m_Width, m_Height, GL_FALSE);
			}
			else
			{
				glBindTexture(GL_TEXTURE_2D, m_DepthStencilAttachment);
				glTexStorage2D(GL_TEXTURE_2D, 1, dataFormat, m_Width, m_Height);
			}
		}
	}
}