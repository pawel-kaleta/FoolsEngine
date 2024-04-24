#pragma once

#include "FoolsEngine\Renderer\1 - Primitives\GDIType.h"
#include "FoolsEngine\Renderer\1 - Primitives\TextureData.h"
#include "FoolsEngine\Assets\AssetManager.h"

#include "OpenGL\OpenGLTexture.h"

namespace fe
{
	class Texture : public ACBody
	{
	public:
		static AssetType GetAssetType() { return AssetType::TextureAsset; }
	};

	class Texture2D : public ACBody
	{
		TextureData::Specification m_Specification;
	};

	class TextureBuilder
	{
	public:
		TextureBuilder() = default;

		TextureBuilder& SetName(const std::string& name) { m_Name = name; return *this; }
		TextureBuilder& SetGDI(GDIType GDI)              { m_GDI  = GDI;  return *this; }

		TextureBuilder& SetSpecification(const TextureData::Specification& specification) { m_Specification = specification; return *this; }

		TextureBuilder& SetResolution(uint32_t width, uint32_t height)    { m_Specification.Width  = width;
		                                                                    m_Specification.Height = height; return *this; }
		TextureBuilder& SetType(TextureData::Type type)                   { m_Specification.Type       = type;       return *this; }
		TextureBuilder& SetUsage(TextureData::Usage usage)                { m_Specification.Usage      = usage;      return *this; }
		TextureBuilder& SetComponents(TextureData::Components components) { m_Specification.Components = components; return *this; }
		TextureBuilder& SetFormat(TextureData::Format format)             { m_Specification.Format     = format;     return *this; }

		AssetID Create();
	private:
		TextureData::Specification m_Specification;
		std::string m_Name;
		GDIType     m_GDI = GDIType::none;
	};
}