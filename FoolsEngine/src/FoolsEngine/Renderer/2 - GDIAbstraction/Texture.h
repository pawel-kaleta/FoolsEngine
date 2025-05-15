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

	struct ACTexture2DData final : public AssetComponent
	{
		TextureData::Specification Specification;
		void* Data;

		void Init()
		{
			Data = nullptr;
			Specification.Init();
		}
	};

	class Texture2D : public Asset
	{
	public:
		virtual AssetType GetType() const override { return GetTypeStatic(); }
		static constexpr AssetType GetTypeStatic() { return AssetType::Texture2DAsset; }

		virtual void PlaceCoreComponent() final override { Emplace<ACTexture2DData>().Init(); }
		virtual void Release() final override;
		void SendDataToGPU(GDIType GDI, void* data);
		void Bind(GDIType GDI, RenderTextureSlotID slotID = 0);
		void UnloadFromCPU();

		uint32_t GetRendererID(GDIType GDI) const;

		const TextureData::Specification& GetSpecification() const { return Get<ACTexture2DData>().Specification; }
		      TextureData::Specification& GetSpecification()       { return Get<ACTexture2DData>().Specification; }

		template <typename tnGDITexture2D>
		tnGDITexture2D& CreateGDITexture2D(const TextureData::Specification& spec, const void* data) { return Emplace<tnGDITexture2D>(spec, data); }

		void CreateGDITexture2D(GDIType gdi)
		{
			auto& data = Get<ACTexture2DData>();
			CreateGDITexture2D(gdi, data.Specification, data.Data);
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
}