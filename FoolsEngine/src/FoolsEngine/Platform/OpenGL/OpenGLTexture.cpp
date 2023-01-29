#include "FE_pch.h"

#include "OpenGLTexture.h"

#include <stb_image.h>


namespace fe
{
	OpenGLTexture2D::OpenGLTexture2D(const std::string& filePath)
		: m_FilePath(filePath), m_Format(0), m_InternalFormat(0)
	{
		m_Name = FileNameFromFilepath(filePath);

		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = stbi_load(filePath.c_str(), &width, &height, &channels, 0);
		FE_LOG_CORE_INFO("Channels: {0}", channels);
		FE_CORE_ASSERT(data, "Failed to load image!");
		m_Width = width;
		m_Height = height;

		if (channels == 3)
		{
			m_Format = GL_RGB;
			m_InternalFormat = GL_RGB8;
		}
		else if (channels == 4)
		{
			m_Format = GL_RGBA;
			m_InternalFormat = GL_RGBA8;
		}

		FE_CORE_ASSERT(m_Format & m_InternalFormat, "Texture data format not supported!");

		glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);
		glTextureStorage2D(m_ID, 1, m_InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureSubImage2D(m_ID, 0, 0, 0, m_Width, m_Height, m_Format, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_ID);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_ID);
	}
}