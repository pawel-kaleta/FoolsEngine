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

		auto& spec = Description::Library::Get().TextureSpecs[SpecificationID];

		glCreateTextures(GL_TEXTURE_2D, 1, &TextureOpenGLID);

		if (spec.Wrapping != Wrapping::Repeat) // default opengl
		{
			switch (spec.Wrapping)
			{
			case Wrapping::MirrorRepeat:
				glTextureParameteri(TextureOpenGLID, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
				glTextureParameteri(TextureOpenGLID, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
				break;
			case Wrapping::Clamp:
				glTextureParameteri(TextureOpenGLID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTextureParameteri(TextureOpenGLID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				break;
			case Wrapping::Border:
				glTextureParameteri(TextureOpenGLID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
				glTextureParameteri(TextureOpenGLID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
				glTextureParameterfv(TextureOpenGLID, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(BorderColor));
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
				case Mipmapping::None:		glTextureParameteri(TextureOpenGLID, GL_TEXTURE_MIN_FILTER, GL_NEAREST); break;
				case Mipmapping::Nearest:	glTextureParameteri(TextureOpenGLID, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST); break;
				case Mipmapping::Liniear:	glTextureParameteri(TextureOpenGLID, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR); break;
				default: FE_LOG_CORE_ERROR("Unrecognized texture Mipmapping mode, defaulted filtering-mipmapping to GL_NEAREST_MIPMAP_LINEAR");
				}
				break;
			case Filtering::Bilinear:
				switch (spec.Mipmapping)
				{
				case Mipmapping::None:		glTextureParameteri(TextureOpenGLID, GL_TEXTURE_MIN_FILTER, GL_LINEAR); break;
				case Mipmapping::Nearest:	glTextureParameteri(TextureOpenGLID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST); break;
				case Mipmapping::Liniear:	glTextureParameteri(TextureOpenGLID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); break;
				default: FE_LOG_CORE_ERROR("Unrecognized texture Mipmapping mode, defaulted filtering-mipmapping to GL_NEAREST_MIPMAP_LINEAR");
				}
				break;
			default:
				FE_LOG_CORE_ERROR("Unrecognized texture Filtering mode, defaulted filtering-mipmapping to GL_NEAREST_MIPMAP_LINEAR");
			}
		}
	}

	void Texture_OpenGL::Destroy()
	{
		FE_PROFILER_FUNC();

		glDeleteTextures(1, &TextureOpenGLID);
	}
}