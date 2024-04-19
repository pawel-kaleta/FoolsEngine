#include "FE_pch.h"

#include "Texture.h"
#include "OpenGL\OpenGLTexture.h"
#include "FoolsEngine\Renderer\9 - Integration\Renderer.h"
#include "FoolsEngine\Assets\Registries.h"

namespace fe
{
	Texture* Texture2D::Create(const std::string& name, const TextureData::Specification& specification, uint32_t width, uint32_t hight, GDIType GDI, AssetSignature* assetSignature)
	{
		switch (GDI)
		{
		case GDIType::none:
			FE_CORE_ASSERT(false, "GDIType::none currently not supported!");
			return nullptr;
		case GDIType::OpenGL:
			return new OpenGLTexture2D(name, specification, width, hight, assetSignature);
		}

		FE_CORE_ASSERT(false, "Unknown GDI");
		return nullptr;
	}
	
	Texture* Texture2D::Create(const std::filesystem::path& filePath, TextureData::Usage usage, AssetSignature* assetSignature)
	{
		GDIType GDI = Renderer::GetActiveGDItype();
		return Create(filePath, GDI, usage, assetSignature);
	}

	Texture* Texture2D::Create(const std::filesystem::path& filePath, GDIType GDI, TextureData::Usage usage, AssetSignature* assetSignature)
	{
		switch (GDI)
		{
		case GDIType::none:
			FE_CORE_ASSERT(false, "GDIType::none currently not supported!");
			return nullptr;
		case GDIType::OpenGL:
			return new OpenGLTexture2D(filePath, usage, assetSignature);
		}

		FE_CORE_ASSERT(false, "Unknown GDI");
		return nullptr;
	}
	

	Texture* TextureBuilder::Create()
	{
		FE_CORE_ASSERT(m_Specification.Components != TextureData::Components::None, "Unspecified components of a texture");
		FE_CORE_ASSERT(m_Specification.Format     != TextureData::Format::None,     "Unspecified format of a texture");
		FE_CORE_ASSERT(m_Width  != 0, "Unspecified width of a texture");
		FE_CORE_ASSERT(m_Height != 0, "Unspecified height of a texture");

		if (m_Specification.Components == TextureData::Components::None) return nullptr;
		if (m_Specification.Format     == TextureData::Format::None    ) return nullptr;
		if (m_Width  == 0) return nullptr;
		if (m_Height == 0) return nullptr;

		if (m_GDI == GDIType::none)
			m_GDI = Renderer::GetActiveGDItype();

		switch (m_Specification.Type)
		{
		case TextureData::Type::None:
			FE_CORE_ASSERT(false, "Unspecified type of a texture");
			return nullptr;
		case TextureData::Type::Texture2D:
			m_Signature = AssetSignatureRegistry::GenerateNew();
			return Texture2D::Create(m_Name, m_Specification, m_Width, m_Height, m_GDI, m_Signature);
		default:
			FE_CORE_ASSERT(false, "Unknown type of a texture");
			return nullptr;
		}
	}
}