#include "FE_pch.h"

#include "FoolsEngine\Renderer\2 - Resource\Texture.h"
#include "FoolsEngine\Renderer\1 - Description\Library.h"

#include <glm\gtc\type_ptr.hpp>

namespace fe::Resource
{
	using namespace Description::Texture;

	GLenum Texture_OpenGL::FormatToGlFormat(Format format)
	{
		// TO DO: make this a static lookup table?

		switch (format.Value)
		{
		case Format::None:
			FE_CORE_ASSERT(false, "Not specified data format of attachment");
			return GL_NONE;
		case Format::R_8:				return GL_RED;
		case Format::RG_8:				return GL_RG;
		case Format::RGB_8:				return GL_RGB;
		case Format::RGBA_8:			return GL_RGBA;
		case Format::R_UINT_32:			return GL_RED_INTEGER;
		case Format::DEPTH24STENCIL8:	return GL_DEPTH24_STENCIL8;
		default:
			FE_CORE_ASSERT(false, "Uknown data format of attachment");
			return GL_NONE;
		}
	}

	GLenum Texture_OpenGL::FormatToGLinternalFormat(Format format)
	{
		// TO DO: make this a static lookup table?

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

	void Texture_OpenGL::Create(const void* data)
	{
		FE_PROFILER_FUNC();

		auto& spec = Description::Library::Get().TextureSpecs[m_SpecificationID];

		auto format = FormatToGlFormat(spec.Format);
		auto internal_format = FormatToGLinternalFormat(spec.Format);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_OpenGLID);

		if (spec.Wrapping != Wrapping::Repeat) // default opengl
		{
			switch (spec.Wrapping)
			{
			case Wrapping::MirrorRepeat:
				glTextureParameteri(m_OpenGLID, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
				glTextureParameteri(m_OpenGLID, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
				break;
			case Wrapping::Clamp:
				glTextureParameteri(m_OpenGLID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTextureParameteri(m_OpenGLID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				break;
			case Wrapping::Border:
				glTextureParameteri(m_OpenGLID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
				glTextureParameteri(m_OpenGLID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
				glTextureParameterfv(m_OpenGLID, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(m_Border));
				break;
			default:
				FE_LOG_CORE_ERROR("Unrecognized texture wrapping mode, defaulted to repeat");
			}
		}

		if (spec.Filtering != Filtering::Nearest || spec.Mipmapping != Mipmapping::Liniear)
		{
			switch (spec.Filtering)
			{
			case Filtering::Nearest:
				switch (spec.Mipmapping)
				{
				case Mipmapping::None:		glTextureParameteri(m_OpenGLID, GL_TEXTURE_MIN_FILTER, GL_NEAREST); break;
				case Mipmapping::Nearest:	glTextureParameteri(m_OpenGLID, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST); break;
				case Mipmapping::Liniear:	glTextureParameteri(m_OpenGLID, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR); break;
				default: FE_LOG_CORE_ERROR("Unrecognized texture Mipmapping mode, defaulted filtering-mipmapping to GL_NEAREST_MIPMAP_LINEAR");
				}
				break;
			case Filtering::Bilinear:
				switch (spec.Mipmapping)
				{
				case Mipmapping::None:		glTextureParameteri(m_OpenGLID, GL_TEXTURE_MIN_FILTER, GL_LINEAR); break;
				case Mipmapping::Nearest:	glTextureParameteri(m_OpenGLID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST); break;
				case Mipmapping::Liniear:	glTextureParameteri(m_OpenGLID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); break;
				default: FE_LOG_CORE_ERROR("Unrecognized texture Mipmapping mode, defaulted filtering-mipmapping to GL_NEAREST_MIPMAP_LINEAR");
				}
				break;
			default:
				FE_LOG_CORE_ERROR("Unrecognized texture Filtering mode, defaulted filtering-mipmapping to GL_NEAREST_MIPMAP_LINEAR");
			}
		}
		glBindTexture(GL_TEXTURE_2D, m_OpenGLID);
		glTexImage2D(GL_TEXTURE_2D, 0, internal_format, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, data);

		if (spec.Mipmapping == Mipmapping::Nearest || spec.Mipmapping == Mipmapping::Liniear)
			glGenerateMipmap(GL_TEXTURE_2D);
	}
}