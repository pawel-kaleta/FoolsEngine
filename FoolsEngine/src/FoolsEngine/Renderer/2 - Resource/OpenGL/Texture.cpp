#include "FE_pch.h"

#include "Utils.h"

#include "FoolsEngine\Renderer\2 - Resource\Texture.h"
#include "FoolsEngine\Renderer\1 - Description\Library.h"

#include <glm\gtc\type_ptr.hpp>

namespace fe::Resource
{
	using namespace Description::Texture;

	void Texture_OpenGL::Create(const void* data)
	{
		FE_PROFILER_FUNC();

		auto& spec = Description::Library::Get().TextureSpecs[m_SpecificationID];

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

		auto format = Utils::FormatToGLFormat(spec.Format);
		auto internal_format = Utils::FormatToGLInternalFormat(spec.Format);

		glBindTexture(GL_TEXTURE_2D, m_OpenGLID);
		glTexImage2D(GL_TEXTURE_2D, 0, internal_format, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, data);

		if (spec.Mipmapping == Mipmapping::Nearest || spec.Mipmapping == Mipmapping::Liniear)
			glGenerateMipmap(GL_TEXTURE_2D);
	}

	void Texture_OpenGL::Destroy()
	{
		FE_PROFILER_FUNC();

		glDeleteTextures(1, &m_OpenGLID);
	}
}