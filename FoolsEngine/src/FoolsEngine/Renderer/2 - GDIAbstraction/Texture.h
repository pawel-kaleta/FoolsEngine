#pragma once

#include "FoolsEngine\Renderer\1 - Primitives\TextureData.h"
#include "FoolsEngine\Assets\Asset.h"

namespace fe
{
	template <typename>
	class AssetHandle;
	template <typename>
	class AssetUser;

	enum class GDIType;

	struct ACTextureSpecification final : public AssetComponent
	{
		
	};

	struct ACTexture2DSpecification final : public AssetComponent
	{
		TextureData::Specification Specification;
	};

	class Texture2D : public Asset
	{
	public:
		virtual AssetType GetType() const override { return GetTypeStatic(); }
		static AssetType GetTypeStatic() { return AssetType::Texture2DAsset; }

		virtual void PlaceCoreComponents() final override;
		virtual void Release() final override;
		void SendDataToGPU(GDIType GDI, void* data);
		void Bind(GDIType GDI, RenderTextureSlotID slotID = 0);
		void UnloadFromCPU();

		uint32_t GetRendererID(GDIType GDI) const;

		const ACTexture2DSpecification& GetSpecification() const { return Get<ACTexture2DSpecification>(); }
		      ACTexture2DSpecification& GetSpecification()       { return Get<ACTexture2DSpecification>(); }

		template <typename tnGDITexture2D>
		tnGDITexture2D& CreateGDITexture2D(const TextureData::Specification& spec, const void* data) { return Emplace<tnGDITexture2D>(spec, data); }

		void CreateGDITexture2D(GDIType gdi)
		{
			auto& spec = GetSpecification().Specification;
			void* data = GetDataLocation().Data;
			CreateGDITexture2D(gdi, spec, data);
		}

		void CreateGDITexture2D(GDIType gdi, const TextureData::Specification& spec, const void* data);

		template <typename tnGDITexture2D>
		tnGDITexture2D& CreateGDITexture2D()
		{
			auto& spec = GetOrEmplaceSpecification().Specification;
			void* data = GetDataLocation().Data;
			return Emplace<tnGDITexture2D>(spec, data);
		}

	protected:
		Texture2D(ECS_AssetHandle ECS_handle) : Asset(ECS_handle) {}
	};

	class Texture2DBuilder
	{
	public:
		Texture2DBuilder() = default;

		Texture2DBuilder& SetDataPtr(void* data) { m_Data = data; return *this; }

		Texture2DBuilder& SetSpecification(const TextureData::Specification& specification) { m_Specification = specification; return *this; }

		//Texture2DBuilder& SetType(TextureData::Type type)                   { m_Specification.Type       = type;       return *this; }
		Texture2DBuilder& SetUsage(TextureData::Usage usage)                { m_Specification.Usage      = usage;      return *this; }
		Texture2DBuilder& SetComponents(TextureData::Components components) { m_Specification.Components = components; return *this; }
		Texture2DBuilder& SetFormat(TextureData::Format format)             { m_Specification.Format     = format;     return *this; }
		Texture2DBuilder& SetResolution(uint32_t width, uint32_t height)    { m_Specification.Width      = width;
																			  m_Specification.Height     = height;     return *this; }

		void Create(AssetUser<Texture2D>& textureUser);
	private:
		TextureData::Specification m_Specification;
		void* m_Data = nullptr;
	};
}