#pragma once

#include "FoolsEngine\Renderer\2 - GDIAbstraction\Texture.h"
#include <glad\glad.h>

namespace fe
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const std::string& name, uint32_t width, uint32_t hight);
		OpenGLTexture2D(const std::string& filePath);
		virtual ~OpenGLTexture2D() override;

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual TextureData::Format GetFormat() const override;

		virtual void SetData(void* data, uint32_t size) override;

		virtual void Bind(uint32_t slot = 0) const override;

		virtual const std::string& GetName() const { return m_Name; }
		virtual uint32_t GetID() const { return m_ID; }

		virtual const std::string& GetFilePath() const override { return m_FilePath; }
	private:
		uint32_t m_Width, m_Height;
		uint32_t m_ID;
		std::string m_FilePath;
		std::string m_Name;
		GLenum m_Format, m_InternalFormat;
	};
}