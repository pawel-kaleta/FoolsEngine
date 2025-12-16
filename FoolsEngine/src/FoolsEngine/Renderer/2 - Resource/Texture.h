#pragma once

#include "FoolsEngine\Renderer\1 - Description\Texture.h"

#include <glad\glad.h>

namespace fe::Resource
{
	using namespace Description::Texture;

	struct TextureBase
	{
		virtual void Create(const Description::Texture::Specification& instance, const void* data) = 0;
		virtual void Destroy() = 0;
	};

	struct Texture_OpenGL final : TextureBase
	{
		GLuint OpenGLID;

		virtual void Create(const Description::Texture::Specification& instance, const void* data) override;
		virtual void Destroy() override;
	};
}