#include "FE_pch.h"
#include "Texture.h"

#include "OpenGL\OpenGLTexture.h"

#include "FoolsEngine\Renderer\1 - Primitives\GDIType.h"
#include "FoolsEngine\Renderer\9 - Integration\Renderer.h"

#include "FoolsEngine\Assets\AssetHandle.h"
#include "FoolsEngine\Assets\Loaders\TextureLoader.h"

namespace fe
{
	void Texture2D::SendDataToGPU(GDIType GDI, void* data)
	{
		switch (GDI)
		{
		case GDIType::none:
			FE_CORE_ASSERT(false, "Unspecified GDIType");
			break;

		case GDIType::OpenGL:
			auto& spec = Get<ACTexture2DData>();
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

	void Texture2D::UnloadFromCPU()
	{
		auto& dataPtr = Get<ACTexture2DData>().Data;
		if (dataPtr)
		{
			TextureLoader::UnloadTexture(dataPtr);
			dataPtr = nullptr;
		}
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

	void Texture2D::Release()
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
}