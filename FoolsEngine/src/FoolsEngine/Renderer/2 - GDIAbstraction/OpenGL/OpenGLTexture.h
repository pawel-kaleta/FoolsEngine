#pragma once

#include "FoolsEngine\Renderer\2 - GDIAbstraction\Texture.h"
#include "FoolsEngine\Assets\AssetManager.h"
#include <glad\glad.h>

namespace fe
{
	class OpenGLTexture2D
	{
	public:
		OpenGLTexture2D(const TextureData::Specification& specification);
		OpenGLTexture2D(TextureData::Specification& specification, const std::filesystem::path& filePath);
		~OpenGLTexture2D();

		void SetData(void* data, uint32_t size, uint32_t width, uint32_t height);

		void Bind(uint32_t slot = 0) const;

		OpenGLID GetOpenGLID() const { return m_ID; }
	private:
		OpenGLID m_ID;

		GLenum m_Format = GL_NONE;
		GLenum m_InternalFormat = GL_NONE;
	};
}