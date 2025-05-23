#pragma once

#include "FoolsEngine\Renderer\1 - Primitives\TextureData.h"
#include "FoolsEngine\Assets\Asset.h"
#include "FoolsEngine\Assets\AssetInterface.h"

namespace fe
{
	enum class GDIType;

	struct ACTexture2DCore final : public AssetComponent
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
		const ACTexture2DCore& GetCoreComponent() const { return Get<ACTexture2DCore>(); }

		uint32_t GetRendererID(GDIType GDI) const;

	protected:
		Texture2DObserver(ECS_AssetHandle ECS_handle) : AssetInterface(ECS_handle) {}
	};

	class Texture2DUser : public Texture2DObserver
	{
	public:
		ACTexture2DCore& GetCoreComponent() const { return Get<ACTexture2DCore>(); }

		void Release() const;

		void SendDataToGPU(GDIType GDI, void* data) const;
		void Bind(GDIType GDI, RenderTextureSlotID slotID = 0) const;
		void UnloadFromCPU() const;

		template <typename tnGDITexture2D>
		tnGDITexture2D& CreateGDITexture2D(const TextureData::Specification& spec, const void* data) { return Emplace<tnGDITexture2D>(spec, data); }

		void CreateGDITexture2D(GDIType gdi) const
		{
			auto& data = Get<ACTexture2DCore>();
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
		using Core = ACTexture2DCore;
	};
}