#pragma once

#include "FoolsEngine/Renderer/1 - Description/GAPIType.h"
#include "FoolsEngine/Renderer/1 - Description/Texture.h"
#include "FoolsEngine/Foundation/Memory/DataTypes.h"
#include "FoolsEngine/Foundation/Memory/Splice.h"

#include <glad/glad.h>

namespace fe::Resource
{
	using namespace Description::Texture;

	struct RTexture
	{
		Description::Texture::Specification Specification;

		virtual void Create(const Description::Texture::Specification& instance, Splice<Byte> data) = 0;
		virtual void Clear(Splice<U32> values) = 0;
		virtual void Clear(Splice<float> values) = 0;
		virtual void Destroy() = 0;
	};

	struct RTexture_OpenGL final : public RTexture
	{
		GLuint OpenGLID;

		virtual void Create(const Description::Texture::Specification& instance, Splice<Byte> data) override;
		virtual void Clear(Splice<U32> values) final override;
		virtual void Clear(Splice<float> values) final override;
		virtual void Destroy() override;
	};
}