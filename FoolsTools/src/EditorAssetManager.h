#pragma once

#include "FoolsEngine.h"
#include "EditorApp.h"

namespace fe
{
	class EditorAssetManager
	{
	public:

		static AssetRegistry* GetRegistry(AssetType type) { return &(Get().m_Registries[type]); }

		template <typename tnAsset>
		static AssetRegistry* GetRegistry() { return GetRegistry(tnAsset::GetTypeStatic()); }

		static auto GetAll(AssetType type) { return &(Get().m_Registries[type]).view<AssetID>(); }

		template <typename tnAsset>
		static auto GetAll() { return GetRegistry(tnAsset::GetTypeStatic())->view<AssetID>(); }

		template <typename tnAsset>
		static AssetID NewAsset()
		{
			auto newID = NewID<tnAsset>();
			InitAsset<tnAsset>(newID);
			return newID;
		}

	private:
		friend class EditorApp;
		EditorAssetManager() = default;

		static EditorAssetManager& Get() { return (*(EditorApp::Get().m_EditorAssetManager.get())); }

		AssetRegistry m_Registries[AssetType::Count];
		std::unordered_map<fe::UUID, AssetID> m_AssetMapByUUID[AssetType::Count];

		template <typename tnAsset>
		static AssetID NewID() { return GetRegistry<tnAsset>()->create(); }

		template <typename tnAsset>
		static void InitAsset(AssetID assetID)
		{
			AssetRegistry* reg = GetRegistry<tnAsset>();

			reg->emplace<ACUUID>(assetID);
			reg->emplace<ACRefsCounters>(assetID);
			reg->emplace<ACDataLocation>(assetID);
			reg->emplace<ACFilepath>(assetID);
		}
	};

	template <typename tnAsset>
	class EditorAssetHandle
	{
	public:
		static_assert(std::is_base_of_v<Asset, tnAsset>, "This is not an asset!");

		EditorAssetHandle() = default;
		EditorAssetHandle(ECS_AssetHandle assetHandle) :
			m_ID(assetHandle.entity())
		{
			if (assetHandle)
				assetHandle.get<ACRefsCounters>().LiveHandles++;
		};
		EditorAssetHandle(AssetID assetID) :
			m_ID(assetID)
		{
			auto ECShandle = GetECSHandle();
			if (ECShandle.valid())
				ECShandle.get<ACRefsCounters>().LiveHandles++;
		};

		~EditorAssetHandle()
		{
			auto ECShandle = GetECSHandle();
			if (ECShandle.valid())
				ECShandle.get<ACRefsCounters>().LiveHandles--;
		}

		EditorAssetHandle(const EditorAssetHandle& other) :
			m_ID(other.m_ID)
		{
			auto ECShandle = GetECSHandle();
			if (ECShandle.valid())
				ECShandle.get<ACRefsCounters>().LiveHandles++;
		};
		EditorAssetHandle(EditorAssetHandle&& other) :
			m_ID(other.m_ID)
		{
			other.m_ID = NullAssetID;
		};
		EditorAssetHandle& operator=(const EditorAssetHandle& other)
		{
			auto ECShandle = GetECSHandle();
			if (ECShandle.valid())
				ECShandle.get<ACRefsCounters>().LiveHandles--;
			m_ID = other.m_ID;
			ECShandle = GetECSHandle();
			if (ECShandle.valid())
				ECShandle.get<ACRefsCounters>().LiveHandles++;

			return *this;
		}
		EditorAssetHandle& operator=(EditorAssetHandle&& other)
		{
			m_ID = other.m_ID;
			other.m_ID = NullAssetID;

			return *this;
		}

		AssetID GetID() const { return m_ID; }
		static AssetType GetType() { return tnAsset::GetTypeStatic(); }

		bool IsValid() const { return (bool)GetECSHandle(); }

		const AssetObserver<tnAsset> Observe() const { return AssetObserver<tnAsset>(GetECSHandle()); }
		      AssetUser    <tnAsset> Use()     const { return AssetUser    <tnAsset>(GetECSHandle()); }
	private:
		AssetID m_ID = NullAssetID;

		ECS_AssetHandle GetECSHandle() const { return ECS_AssetHandle(*EditorAssetManager::GetRegistry(GetType()), m_ID); };
	};
}