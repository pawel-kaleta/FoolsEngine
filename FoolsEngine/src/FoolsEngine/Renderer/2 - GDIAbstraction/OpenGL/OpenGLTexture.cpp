#include "FE_pch.h"

#include "OpenGLTexture.h"

#include <stb_image.h>


namespace fe
{
	GLenum ComponentsToGlFormat(TextureData::Components components)
	{
		// TO DO: make this a static lookup table

		switch (components)
		{
		case TextureData::Components::None:
			FE_CORE_ASSERT(false, "Unspecified texture components");
			return GL_R;
		case TextureData::Components::RGB_F:
			return GL_RGB;
		case TextureData::Components::RGBA_F:
			return GL_RGBA;
		default:
			FE_CORE_ASSERT(false, "Uknown texture format");
			return GL_R;
		}
	}

	GLenum FormatToGLinternalFormat(TextureData::Format format)
	{
		// TO DO: make this a static lookup table

		switch (format)
		{
		case TextureData::Format::None:
			FE_CORE_ASSERT(false, "Unspecified texture data format");
			return GL_R8;
		case TextureData::Format::RGB_FLOAT_8:
			return GL_RGB8;
		case TextureData::Format::RGBA_FLOAT_8:
			return GL_RGBA8;
		default:
			FE_CORE_ASSERT(false, "Uknown texture data format");
			return GL_R8;
		}
	}

	OpenGLTexture2D::OpenGLTexture2D(const TextureData::Specification& specification)
	{
		m_Format = ComponentsToGlFormat(specification.Components);
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

		m_Format = ComponentsToGlFormat(spec.Components);
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