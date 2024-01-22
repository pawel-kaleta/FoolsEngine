#include "FE_pch.h"

#include "OpenGLTexture.h"

#include <stb_image.h>


namespace fe
{
	OpenGLTexture2D::OpenGLTexture2D(const std::string& name, uint32_t width, uint32_t hight)
		: m_Name(name), m_Width(width), m_Height(hight)
	{
		m_InternalFormat = GL_RGBA8;
		m_Format = GL_RGBA;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);
		glTextureStorage2D(m_ID, 1, m_InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& filePath)
		: m_FilePath(filePath), m_Format(0), m_InternalFormat(0)
	{
		m_Name = FileNameFromFilepath(filePath);
		m_FilePath = filePath;

		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = stbi_load(filePath.c_str(), &width, &height, &channels, 0);
		FE_LOG_CORE_DEBUG("Loading texture, Channels: {0}", channels);
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

		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(m_ID, 0, 0, 0, m_Width, m_Height, m_Format, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_ID);
	}

	TextureData::Format OpenGLTexture2D::GetFormat() const
	{
		switch (m_Format)
		{
		case GL_RGB:
			return TextureData::Format::RGB;
		case GL_RGBA:
			return TextureData::Format::RGBA;
		}
		return TextureData::Format::None;
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		// bytes per pixel
		uint32_t bpp = m_Format == GL_RGBA ? 4 : 3;
		FE_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be entire texture!");
		glTextureSubImage2D(m_ID, 0, 0, 0, m_Width, m_Height, m_Format, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_ID);
	}
}