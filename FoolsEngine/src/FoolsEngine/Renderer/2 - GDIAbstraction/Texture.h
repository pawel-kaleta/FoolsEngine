#pragma once

#include "FoolsEngine\Renderer\1 - Primitives\TextureData.h"
#include "FoolsEngine\Assets\Asset.h"

namespace fe
{
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

	class Texture2DObserver : public AssetInterface
	{
	public:
		virtual AssetType GetType() const override { return GetTypeStatic(); }
		static constexpr AssetType GetTypeStatic() { return AssetType::Texture2DAsset; }

		uint32_t GetRendererID(GDIType GDI) const;

		const ACTexture2DData& GetDataComponent() const { return Get<ACTexture2DData>(); }
	protected:
		Texture2DObserver(ECS_AssetHandle ECS_handle) : AssetInterface(ECS_handle) {}
	};

	class Texture2DUser : public Texture2DObserver
	{
	public:
		ACTexture2DData& GetDataComponent() const { return Get<ACTexture2DData>(); }

		void PlaceCoreComponent() const { Emplace<ACTexture2DData>().Init(); }
		void Release() const;

		void SendDataToGPU(GDIType GDI, void* data) const;
		void Bind(GDIType GDI, RenderTextureSlotID slotID = 0) const;
		void UnloadFromCPU() const;

		template <typename tnGDITexture2D>
		tnGDITexture2D& CreateGDITexture2D(const TextureData::Specification& spec, const void* data) { return Emplace<tnGDITexture2D>(spec, data); }

		void CreateGDITexture2D(GDIType gdi) const
		{
			auto& data = Get<ACTexture2DData>();
			CreateGDITexture2D(gdi, data.Specification, data.Data);
		}

		void CreateGDITexture2D(GDIType gdi, const TextureData::Specification& spec, const void* data) const;

		template <typename tnGDITexture2D>
		tnGDITexture2D& CreateGDITexture2D() const
		{
			auto& spec = GetOrEmplaceSpecification().Specification;
			void* data = GetDataLocation().Data;
			return Emplace<tnGDITexture2D>(spec, data);
		}

	protected:
		Texture2DUser(ECS_AssetHandle ECS_handle) : Texture2DObserver(ECS_handle) {}
	};

	class Texture2D : public Asset
	{
	public:
		static constexpr AssetType GetTypeStatic() { return AssetType::Texture2DAsset; }

		using Observer = Texture2DObserver;
		using User = Texture2DUser;
	};
}