#pragma once

#include "FoolsEngine\Renderer\1 - Description\Texture.h"

#include <glad\glad.h>
#include <glm/glm.hpp>

namespace fe::Resource
{
	using namespace Description::Texture;

	struct TextureBase
	{
		uint32_t SpecificationID;
		uint32_t Width;
		uint32_t Height;
		Usage Usage;
		glm::vec3 BorderColor;

		virtual void Create(const void* data) = 0;
		virtual void Destroy() = 0;
	};

	struct Texture_OpenGL final : TextureBase
	{
		GLuint OpenGLID;

		virtual void Create(const void* data) override;
		virtual void Destroy() override;
	};
}