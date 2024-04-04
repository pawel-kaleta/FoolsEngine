#pragma once

#include "FoolsEngine\Renderer\1 - Primitives\GDIType.h"
#include "FoolsEngine\Renderer\1 - Primitives\TextureData.h"
#include "FoolsEngine\Assets\Asset.h"

namespace fe
{
	class Texture : public Asset
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t                GetWidth()      const = 0;
		virtual uint32_t                GetHeight()     const = 0;
		virtual TextureData::Type       GetType()       const = 0;
		virtual TextureData::Usage      GetUsage()      const = 0;
		virtual TextureData::Components GetComponents() const = 0;
		virtual TextureData::Format     GetFormat()     const = 0;
		virtual const std::string&      GetName()       const = 0;
		virtual const std::string&      GetFilePath()   const = 0;
		virtual uint32_t                GetID()         const = 0;

		virtual void SetData(void* data, uint32_t size) = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;

		static AssetType GetAssetType() { return AssetType::TextureAsset; }
	};

	class Texture2D : public Texture
	{
	public:
		virtual ~Texture2D() = default;

		virtual TextureData::Type GetType() const override { return TextureData::Type::Texture2D; }

		static Texture* Create(const std::string& filePath, TextureData::Usage usage);
		static Texture* Create(const std::string& filePath, GDIType GDI, TextureData::Usage usage);
	private:
		friend class TextureBuilder;
		static Texture* Create(
			const std::string& name,
			const TextureData::Specification& specification,
			uint32_t width,
			uint32_t hight, GDIType GDI,
			const AssetSignature& assetSignature);
	};

	class TextureBuilder
	{
	public:
		TextureBuilder() = default;
		TextureBuilder(const TextureData::Specification& specification)
			: m_Specification(specification) { }

		TextureBuilder& SetName(const std::string& name) { m_Name   = name;   return *this; }
		TextureBuilder& SetWidth(uint32_t width)         { m_Width  = width;  return *this; }
		TextureBuilder& SetHeight(uint32_t height)       { m_Height = height; return *this; }
		TextureBuilder& SetGDI(GDIType GDI)              { m_GDI    = GDI;    return *this; }

		TextureBuilder& SetType(TextureData::Type type)                   { m_Specification.Type       = type;       return *this; }
		TextureBuilder& SetUsage(TextureData::Usage usage)                { m_Specification.Usage      = usage;      return *this; }
		TextureBuilder& SetComponents(TextureData::Components components) { m_Specification.Components = components; return *this; }
		TextureBuilder& SetFormat(TextureData::Format format)             { m_Specification.Format     = format;     return *this; }

		TextureBuilder& SetSignature(const AssetSignature& assetSignature) { m_Signature = assetSignature; }

		Texture* Create();
	private:
		TextureData::Specification m_Specification;
		AssetSignature m_Signature;
		std::string    m_Name;
		uint32_t       m_Width  = 0;
		uint32_t       m_Height = 0;
		GDIType        m_GDI    = GDIType::none;
	};
}