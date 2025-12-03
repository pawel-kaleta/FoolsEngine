#pragma once

#include "FoolsEngine\Renderer\1 - Description\Texture.h"

#include <glad\glad.h>
#include <glm/glm.hpp>

namespace fe::Resource
{
	using namespace Description::Texture;

	struct TextureBase
	{
		uint32_t m_SpecificationID;
		uint32_t m_Width;
		uint32_t m_Height;
		Usage m_Usage;
		glm::vec3 m_Border;

		virtual void Create(const void* data) = 0;
	};

	struct Texture_OpenGL final : TextureBase
	{
		GLuint m_OpenGLID;

		static GLenum FormatToGlFormat(Format format);
		static GLenum FormatToGLinternalFormat(Format format);

		virtual void Create(const void* data) override;
	};
}