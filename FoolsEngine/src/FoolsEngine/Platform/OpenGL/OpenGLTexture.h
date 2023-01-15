#pragma once

#include "FoolsEngine\Renderer\APIAbstraction\Texture.h"

namespace fe
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const std::string& filePath);
		virtual ~OpenGLTexture2D();

		virtual uint32_t GetWidth() const override { return m_Width; };
		virtual uint32_t GetHeight() const override { return m_Height; };
		//virtual TextureFormat GetFormat() const = 0;

		virtual void Bind(uint32_t slot = 0) const override;

		inline const std::string& GetFilePath() const { return m_FilePath; }
	private:
		uint32_t m_Width, m_Height;
		uint32_t m_ID;
		std::string m_FilePath;
	};
}