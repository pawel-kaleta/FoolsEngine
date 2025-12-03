#include "FE_pch.h"
#include "OpenGLTexture.h"

#include "FoolsEngine\Renderer\1 - Description\Texture.h"

#include <glad\glad.h>

namespace fe
{
	static GLenum FormatToGlFormat(Description::Texture::Format format)
	{
		// TO DO: make this a static lookup table?

		switch (format.Value)
		{
		case Description::Texture::Format::None:
			FE_CORE_ASSERT(false, "Not specified data format of attachment");
			return GL_NONE;
		case Description::Texture::Format::R_8:				return GL_RED;
		case Description::Texture::Format::RG_8:			return GL_RG;
		case Description::Texture::Format::RGB_8:			return GL_RGB;
		case Description::Texture::Format::RGBA_8:			return GL_RGBA;
		case Description::Texture::Format::R_UINT_32:		return GL_RED_INTEGER;
		case Description::Texture::Format::DEPTH24STENCIL8:	return GL_DEPTH24_STENCIL8;
		default:
			FE_CORE_ASSERT(false, "Uknown data format of attachment");
			return GL_NONE;
		}
	}

	static GLenum FormatToGLinternalFormat(Description::Texture::Format format)
	{
		// TO DO: make this a static lookup table?

		switch (format.Value)
		{
		case Description::Texture::Format::None:
			FE_CORE_ASSERT(false, "Not specified data format of attachment");
			return GL_NONE;
		case Description::Texture::Format::R_8:				return GL_R8;
		case Description::Texture::Format::RG_8:			return GL_RG8;
		case Description::Texture::Format::RGB_8:			return GL_RGB8;
		case Description::Texture::Format::RGBA_8:			return GL_RGBA8;
		case Description::Texture::Format::R_UINT_32:		return GL_R32UI;
		case Description::Texture::Format::DEPTH24STENCIL8:	return GL_DEPTH24_STENCIL8;
		default:
			FE_CORE_ASSERT(false, "Uknown data format of attachment");
			return GL_NONE;
		}
	}

	OpenGLTexture2D::OpenGLTexture2D(const Description::Texture::Specification& specification, uint32_t width, uint32_t height)
	{
		FE_PROFILER_FUNC();

		m_Format = FormatToGlFormat(specification.Format);
		m_InternalFormat = FormatToGLinternalFormat(specification.Format);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);
		glTextureStorage2D(m_ID, 1, m_InternalFormat, width, height);

		glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	OpenGLTexture2D::OpenGLTexture2D(const Description::Texture::Specification& spec, const void* data, uint32_t width, uint32_t height)
	{
		FE_PROFILER_FUNC();

		FE_CORE_ASSERT(data, "No texture data");

		m_Format = FormatToGlFormat(spec.Format);
		m_InternalFormat = FormatToGLinternalFormat(spec.Format);

		{
			FE_PROFILER_SCOPE("GPU Texture Creation");

			glGenTextures(1, &m_ID);
			glBindTexture(GL_TEXTURE_2D, m_ID);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}

		{
			FE_PROFILER_SCOPE("GPU Texture Upload");
			glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, width, height, 0, m_Format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		FE_PROFILER_FUNC();
		if(m_ID) // objects that where moved out from are still destroyed 
		{
			FE_LOG_CORE_DEBUG("Unloading OpenGLTexture from GPU, RendererID: {0}", m_ID);
			glDeleteTextures(1, &m_ID);
		}
	}

	void OpenGLTexture2D::SendDataToGPU(void* data, const Description::Texture::Specification& spec, uint32_t width, uint32_t height)
	{
		FE_PROFILER_FUNC();
		FE_CORE_ASSERT(m_ID, "TextureID == 0");
		glTextureSubImage2D(m_ID, 0, 0, 0, width, height, m_Format, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::Bind(RenderTextureSlotID slotID) const
	{
		FE_PROFILER_FUNC();
		FE_CORE_ASSERT(m_ID, "TextureID == 0");
		glBindTextureUnit(slotID, m_ID);
	}
}