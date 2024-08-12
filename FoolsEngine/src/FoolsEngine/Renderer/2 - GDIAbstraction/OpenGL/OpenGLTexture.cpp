#include "FE_pch.h"

#include "OpenGLTexture.h"

#include "FoolsEngine\Renderer\1 - Primitives\TextureData.h"

#include <glad\glad.h>

namespace fe
{
	GLenum FormatToGlFormat(TextureData::Format format)
	{
		// TO DO: make this a static lookup table

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

	GLenum FormatToGLinternalFormat(TextureData::Format format)
	{
		// TO DO: make this a static lookup table

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

	OpenGLTexture2D::OpenGLTexture2D(const TextureData::Specification& specification)
	{
		m_Format = FormatToGlFormat(specification.Format);
		m_InternalFormat = FormatToGLinternalFormat(specification.Format);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);
		glTextureStorage2D(m_ID, 1, m_InternalFormat, specification.Width, specification.Height);

		glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	OpenGLTexture2D::OpenGLTexture2D(const TextureData::Specification& spec, const void* data)
	{
		FE_CORE_ASSERT(data, "No texture data");

		m_Format = FormatToGlFormat(spec.Format);
		m_InternalFormat = FormatToGLinternalFormat(spec.Format);

		glGenTextures(1, &m_ID);
		glBindTexture(GL_TEXTURE_2D, m_ID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, spec.Width, spec.Height, 0, m_Format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		FE_LOG_CORE_DEBUG("Uploding texture to GPU, RendererID: {0}", m_ID);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		if(m_ID)
		{
			auto x = m_ID;
			FE_LOG_CORE_DEBUG("Unloading OpenGLTexture from GPU, RendererID: {0}", x);
			glDeleteTextures(1, &m_ID);
		}
	}

	void OpenGLTexture2D::SendDataToGPU(void* data, const TextureData::Specification& spec)
	{
		glTextureSubImage2D(m_ID, 0, 0, 0, spec.Width, spec.Height, m_Format, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::Bind(RenderTextureSlotID slotID) const
	{
		glBindTextureUnit(slotID, m_ID);
	}
}