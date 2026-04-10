#include "FE_pch.h"

#include "Utils.h"

#include "FoolsEngine/Renderer/1 - Description/Library.h"
#include "FoolsEngine/Renderer/2 - Resource/RTexture.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/exponential.hpp>

namespace fe::Resource
{
	using namespace Description::Texture;

	void RTexture_OpenGL::Create(const Description::Texture::Specification& instance, Splice<Byte> data)
	{
		FE_PROFILER_FUNC();

		Specification = instance;
		auto& arch = Description::Library::Get().TextureArchetypes[instance.ArchetypeID];

		glCreateTextures(GL_TEXTURE_2D, 1, &OpenGLID);

		if (arch.Wrapping != Wrapping::Repeat) // default opengl
		{
			switch (arch.Wrapping)
			{
			case Wrapping::MirrorRepeat:
				glTextureParameteri(OpenGLID, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
				glTextureParameteri(OpenGLID, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
				break;
			case Wrapping::Clamp:
				glTextureParameteri(OpenGLID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTextureParameteri(OpenGLID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				break;
			case Wrapping::Border:
				glTextureParameteri(OpenGLID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
				glTextureParameteri(OpenGLID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
				glTextureParameterfv(OpenGLID, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(instance.BorderColor));
				break;
			default:
				FE_LOG_CORE_ERROR("Unrecognized texture wrapping mode, defaulted to repeat");
			}
		}

		if (arch.Filtering != Filtering::Nearest || arch.Mipmapping != Mipmapping::Liniear)
		{
			switch (arch.Filtering)
			{
			case Filtering::Nearest:
				switch (arch.Mipmapping)
				{
				case Mipmapping::None:		glTextureParameteri(OpenGLID, GL_TEXTURE_MIN_FILTER, GL_NEAREST); break;
				case Mipmapping::Nearest:	glTextureParameteri(OpenGLID, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST); break;
				case Mipmapping::Liniear:	glTextureParameteri(OpenGLID, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR); break;
				default: FE_LOG_CORE_ERROR("Unrecognized texture Mipmapping mode, defaulted filtering-mipmapping to GL_NEAREST_MIPMAP_LINEAR");
				}
				break;
			case Filtering::Bilinear:
				switch (arch.Mipmapping)
				{
				case Mipmapping::None:		glTextureParameteri(OpenGLID, GL_TEXTURE_MIN_FILTER, GL_LINEAR); break;
				case Mipmapping::Nearest:	glTextureParameteri(OpenGLID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST); break;
				case Mipmapping::Liniear:	glTextureParameteri(OpenGLID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); break;
				default: FE_LOG_CORE_ERROR("Unrecognized texture Mipmapping mode, defaulted filtering-mipmapping to GL_NEAREST_MIPMAP_LINEAR");
				}
				break;
			default:
				FE_LOG_CORE_ERROR("Unrecognized texture Filtering mode, defaulted filtering-mipmapping to GL_NEAREST_MIPMAP_LINEAR");
			}
		}

		GLsizei levels = glm::log2((float)glm::max(instance.Width, instance.Height));
		auto internal_format = Utils::FormatToGLInternalFormat(arch.Format);

		glTextureStorage2D(OpenGLID, levels, internal_format, instance.Width, instance.Height);

		auto format = Utils::FormatToGLFormat(arch.Format);
		glTextureSubImage2D(OpenGLID, 0, 0, 0, instance.Width, instance.Height, format, GL_UNSIGNED_BYTE, data.Elements);

		glGenerateTextureMipmap(OpenGLID);
	}

	void RTexture_OpenGL::Clear(Splice<U32> values)
	{
		FE_PROFILER_FUNC();

		const auto& arch = Description::Library::Get().TextureArchetypes[Specification.ArchetypeID];

		FE_CORE_ASSERT(!Description::Texture::IsDepthOrStencil(arch.Format), "This is a depth and/or stencil texture!");
		FE_CORE_ASSERT(Description::Texture::ComponentsCountInFormat(arch.Format) == values.Count, "Mismach between number of provided values and number of components in color attachment!");

		glClearTexImage(OpenGLID, 0, Resource::Utils::FormatToGLFormat(arch.Format), GL_UNSIGNED_INT, values.Elements);
	}

	void RTexture_OpenGL::Clear(Splice<float> values)
	{
		FE_PROFILER_FUNC();

		const auto& arch = Description::Library::Get().TextureArchetypes[Specification.ArchetypeID];

		FE_CORE_ASSERT(!Description::Texture::IsDepthOrStencil(arch.Format), "This is a depth and/or stencil texture!");
		FE_CORE_ASSERT(Description::Texture::ComponentsCountInFormat(arch.Format) == values.Count, "Mismach between number of provided values and number of components in color attachment!");

		glClearTexImage(OpenGLID, 0, Resource::Utils::FormatToGLFormat(arch.Format), GL_FLOAT, values.Elements);
	}

	void RTexture_OpenGL::Destroy()
	{
		FE_PROFILER_FUNC();

		glDeleteTextures(1, &OpenGLID);
	}
}