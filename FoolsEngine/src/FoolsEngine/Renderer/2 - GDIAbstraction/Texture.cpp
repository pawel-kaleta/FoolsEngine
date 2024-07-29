#include "FE_pch.h"

#include "Texture.h"
#include "OpenGL\OpenGLTexture.h"
#include "FoolsEngine\Assets\Loaders\TextureLoader.h"
#include "FoolsEngine\Renderer\9 - Integration\Renderer.h"

namespace fe
{
	AssetHandle<Texture2D> Texture2D::GetHandle() const { return AssetHandle<Texture2D>(GetECSHandle()); }

	void Texture2D::SendDataToGPU(GDIType GDI, void* data)
	{
		switch (GDI)
		{
		case GDIType::none:
			FE_CORE_ASSERT(false, "Unspecified GDIType");
			break;

		case GDIType::OpenGL:
			auto& spec = Get<ACTexture2DSpecification>();
			Get<OpenGLTexture2D>().SendDataToGPU(data, spec.Specification);
			break;
		}
	}

	void Texture2D::Bind(GDIType GDI, RenderTextureSlotID slotID)
	{
		switch (GDI)
		{
		case GDIType::none:
			FE_CORE_ASSERT(false, "Unspecified GDIType");
			break;

		case GDIType::OpenGL:
			auto ptr = GetIfExist<OpenGLTexture2D>();
			FE_CORE_ASSERT(ptr, "Trying to bind texture not loaded to GPU");
			if (ptr)
				ptr->Bind(slotID);
			break;
		}
	}

	void Texture2D::UnloadFromGPU()
	{
		auto gdi = Renderer::GetActiveGDItype();
		switch (gdi)
		{
		case GDIType::none:
			FE_CORE_ASSERT(false, "Unspecified GDIType");
			break;

		case GDIType::OpenGL:
			if (AllOf<OpenGLTexture2D>())
			{
				FE_LOG_CORE_DEBUG("Unloading Texture from GPU, AssetID: {0}, RendererID: {1}", GetID(), GetRendererID(gdi));
				Erase<OpenGLTexture2D>();
			}
			break;
		}
	}

	void Texture2D::UnloadFromCPU()
	{
		auto& dataPtr = GetDataLocation().Data;
		if (!dataPtr)
			return;
		stbi_image_free(dataPtr);
		dataPtr = nullptr;
	}

	uint32_t Texture2D::GetRendererID(GDIType GDI) const
	{
		switch (GDI)
		{
		case GDIType::none:
			FE_CORE_ASSERT(false, "Unspecified GDIType");
			return 0;

		case GDIType::OpenGL:
			auto ptr = GetIfExist<OpenGLTexture2D>();
			if (ptr)
				return ptr->GetOpenGLID();
			return 0;
		}
		return 0;
	}

	void Texture2D::CreateGDITexture2D(GDIType gdi, const TextureData::Specification& spec, const void* data)
	{
		switch (gdi)
		{
		case GDIType::none:
			FE_CORE_ASSERT(false, "Unspecified GDIType");
			break;

		case GDIType::OpenGL:
			Emplace<OpenGLTexture2D>(spec, data);
			break;
		}
	}

	void Texture2DBuilder::Create(AssetUser<Texture2D>& textureUser)
	{
		FE_CORE_ASSERT(m_Data, "Texture data pointer not set");
		if (!m_Data) return;

		FE_CORE_ASSERT(m_Specification.Components != TextureData::Components::None, "Unspecified components of a texture");
		FE_CORE_ASSERT(m_Specification.Format     != TextureData::Format::None,     "Unspecified format of a texture");
		FE_CORE_ASSERT(m_Specification.Width      != 0,                             "Unspecified width of a texture");
		FE_CORE_ASSERT(m_Specification.Height     != 0,                             "Unspecified height of a texture");

		if (m_Specification.Components == TextureData::Components::None) return;
		if (m_Specification.Format     == TextureData::Format::None    ) return;
		if (m_Specification.Width      == 0                            ) return;
		if (m_Specification.Height     == 0                            ) return;

		textureUser.GetDataLocation().Data = m_Data;

		switch (m_Specification.Type)
		{
		case TextureData::Type::None:
			FE_CORE_ASSERT(false, "Unspecified type of a texture");
			return;

		case TextureData::Type::Texture2D:
		{
			textureUser.GetOrEmplaceSpecification().Specification = m_Specification;
			return;
		}
		default:
			FE_CORE_ASSERT(false, "Unknown type of a texture");
			return;
		}
	}
}