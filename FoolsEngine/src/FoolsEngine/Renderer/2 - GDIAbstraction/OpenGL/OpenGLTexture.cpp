#include "FE_pch.h"

#include "OpenGLTexture.h"

#include <stb_image.h>


namespace fe
{
	OpenGLTexture2D::OpenGLTexture2D(const std::string& name, TextureData::Specification specification, uint32_t width, uint32_t hight, AssetSignature* assetSignature)
		: m_Name(name), m_Width(width), m_Height(hight), m_Usage(specification.Usage)
	{
		// TO DO: factor out specification translation abstract<->OpenGL
		m_Signature = assetSignature;

		switch (specification.Components)
		{
		case TextureData::Components::None:
			FE_CORE_ASSERT(false, "Unspecified texture format");
			break;
		case TextureData::Components::RGB_F:
			m_Format = GL_RGB;
			break;
		case TextureData::Components::RGBA_F:
			m_Format = GL_RGBA;
			break;
		default:
			FE_CORE_ASSERT(false, "Uknown texture format");
		}

		switch (specification.Format)
		{
		case TextureData::Format::None:
			FE_CORE_ASSERT(false, "Unspecified texture data format");
			break;
		case TextureData::Format::RGB_FLOAT_8:
			m_InternalFormat = GL_RGB8;
			break;
		case TextureData::Format::RGBA_FLOAT_8:
			m_InternalFormat = GL_RGBA8;
			break;
		default:
			FE_CORE_ASSERT(false, "Uknown texture data format");
		}

		glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);
		glTextureStorage2D(m_ID, 1, m_InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::filesystem::path& filePath, TextureData::Usage usage, AssetSignature* assetSignature)
		: m_Format(0), m_InternalFormat(0), m_Usage(usage)
	{
		m_Signature = assetSignature;
		assetSignature->FilePath = filePath;

		m_Name = FileNameFromFilepath(filePath.generic_string());

		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = stbi_load(filePath.generic_string().c_str(), &width, &height, &channels, 0);
		FE_LOG_CORE_DEBUG("Loading texture, Channels: {0}", channels);
		FE_CORE_ASSERT(data, "Failed to load image!");
		m_Width = width;
		m_Height = height;

		switch (channels)
		{
		case 1:
			m_Format = GL_R;
			m_InternalFormat = GL_R32F;
		case 3:
			m_Format = GL_RGB;
			m_InternalFormat = GL_RGB8;
			break;
		case 4:
			m_Format = GL_RGBA;
			m_InternalFormat = GL_RGBA8;
			break;
		}

		FE_CORE_ASSERT(m_Format & m_InternalFormat, "Texture data format not supported!");

		glGenTextures(1, &m_ID);
		glBindTexture(GL_TEXTURE_2D, m_ID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, width, height, 0, m_Format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_ID);
	}

	TextureData::Components OpenGLTexture2D::GetComponents() const
	{
		switch (m_Format)
		{
		case GL_RGB:  return TextureData::Components::RGB_F;
		case GL_RGBA: return TextureData::Components::RGBA_F;
		}
		return TextureData::Components::None;
	}

	TextureData::Format OpenGLTexture2D::GetFormat() const
	{
		switch (m_InternalFormat)
		{
		case GL_RGB8:  return TextureData::Format::RGB_FLOAT_8;
		case GL_RGBA8: return TextureData::Format::RGBA_FLOAT_8;
		}
		return TextureData::Format::None;
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		uint32_t bytes_per_pixel = 0;
		switch (m_Format)
		{
		case GL_RGB:  bytes_per_pixel = 3; break;
		case GL_RGBA: bytes_per_pixel = 4; break;
		default:
		{
			FE_CORE_ASSERT(false, "Uknown data format");
			FE_LOG_CORE_ERROR("Uknown data format of a texture {0}", m_Name);
			return;
		}
		}
		FE_CORE_ASSERT(size == m_Width * m_Height * bytes_per_pixel, "Data must be entire texture!");
		glTextureSubImage2D(m_ID, 0, 0, 0, m_Width, m_Height, m_Format, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_ID);
	}
}