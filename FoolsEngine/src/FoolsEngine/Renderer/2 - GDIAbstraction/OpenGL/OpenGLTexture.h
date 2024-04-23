#pragma once

#include "FoolsEngine\Renderer\2 - GDIAbstraction\Texture.h"
#include "FoolsEngine\Assets\Asset.h"
#include <glad\glad.h>

namespace fe
{
	class OpenGLTexture2D final : public Texture2D
	{
	public:
		OpenGLTexture2D(const std::string& name, TextureData::Specification specification, uint32_t width, uint32_t hight);
		OpenGLTexture2D(const std::filesystem::path& filePath, TextureData::Usage usage);
		~OpenGLTexture2D();



		TextureData::Components GetComponents() const ;
		TextureData::Format     GetFormat()     const ;

		void SetData(void* data, uint32_t size) override;

		void Bind(uint32_t slot = 0) const override;

		uint32_t GetInRenderContextID() const { return m_ID; }
	private:

		uint32_t m_ID;

		GLenum m_Format = GL_NONE;
		GLenum m_InternalFormat = GL_NONE;
		
}