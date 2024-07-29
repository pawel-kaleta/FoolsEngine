#pragma once

#include "FoolsEngine\Renderer\2 - GDIAbstraction\Texture.h"
#include "OpenGLDeviceAPI.h"
#include "FoolsEngine\Assets\AssetManager.h"
#include <glad\glad.h>

namespace fe
{
	class OpenGLTexture2D final : public AssetComponent
	{
	public:
		OpenGLTexture2D(const TextureData::Specification& specification);
		OpenGLTexture2D(const TextureData::Specification& spec, const void* data);
		~OpenGLTexture2D();

		OpenGLTexture2D(OpenGLTexture2D&& other) { m_ID = other.m_ID; m_Format = other.m_Format; m_InternalFormat = other.m_InternalFormat; other.m_ID = 0; };
		OpenGLTexture2D(const OpenGLTexture2D& other) = delete;
		OpenGLTexture2D& operator=(OpenGLTexture2D&& other) { m_ID = other.m_ID; m_Format = other.m_Format; m_InternalFormat = other.m_InternalFormat; other.m_ID = 0; return *this; };
		OpenGLTexture2D& operator=(const OpenGLTexture2D& other) = delete;

		void SendDataToGPU(void* data, const TextureData::Specification& spec);

		void Bind(RenderTextureSlotID slotID = 0) const;

		OpenGLID GetOpenGLID() const { return m_ID; }

		uint32_t GetID() const { return m_ID; }
	private:
		OpenGLID m_ID = 0;

		GLenum m_Format = GL_NONE;
		GLenum m_InternalFormat = GL_NONE;
	};
}