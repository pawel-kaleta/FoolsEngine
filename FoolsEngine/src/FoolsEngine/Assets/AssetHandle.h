#pragma once

#include "AssetAccessors.h"

namespace fe
{
	enum AssetLoadingPriority : uint32_t
	{
		LoadingPriority_None = 0,
		LoadingPriority_Low = 1,
		LoadingPriority_Standard = 10,
		LoadingPriority_High = 100,
		LoadingPriority_Critical = (uint32_t)-1
	};

	class AssetHandleBase
	{
	public:
		AssetID GetID() const { return m_ID; }
		AssetLoadingPriority GetLoadingPriority() const { return m_LoadingPriority; }
	protected:
		AssetHandleBase() :
			m_ID(NullAssetID) { };
		AssetHandleBase(AssetID id, AssetLoadingPriority priority) :
			m_ID(id), m_LoadingPriority(priority) { };
		
		AssetID m_ID;
		AssetLoadingPriority m_LoadingPriority;
	};

	template <typename tnAsset>
	class AssetHandle : public AssetHandleBase
	{
	public:
		static_assert(std::is_base_of_v<Asset, tnAsset>, "This is not an asset!");
		// TO DO: add AssetLoadingPriority counting

		static AssetType GetTypeStatic() { return tnAsset::GetTypeStatic(); }

		AssetHandle() { };
		AssetHandle(AssetID assetID, AssetLoadingPriority priority = LoadingPriority_Standard) :
			AssetHandleBase(assetID, priority)
		{
			Init();
		};

		~AssetHandle()
		{
			Deinit();
		}

		AssetHandle(const AssetHandle& other) :
			AssetHandleBase(other.m_ID, other.m_LoadingPriority)
		{
			Init();
		};
		AssetHandle(AssetHandle&& other) :
			AssetHandleBase(other.m_ID, other.m_LoadingPriority)
		{
			other.m_ID = NullAssetID;
		};
		AssetHandle& operator=(const AssetHandle& other)
		{
			Deinit();

			m_ID = other.m_ID;
			m_LoadingPriority = other.m_LoadingPriority;

			Init();

			return *this;
		}
		AssetHandle& operator=(AssetHandle&& other)
		{
			Deinit();

			m_ID = other.m_ID;
			m_LoadingPriority = other.m_LoadingPriority;

			other.m_ID = NullAssetID;

			return *this;
		}

		bool operator==(const AssetHandle& other) const { return m_ID == other.m_ID; }
		
		UUID GetUUID() const { return GetECSHandle().get<ACUUID>().UUID; } // ??? Do we have UUID on every asset?
		bool IsValid() const { return (bool)GetECSHandle(); }
		void SetLoadingPriority(AssetLoadingPriority priority)
		{
			auto refs = GetECSHandle().try_get<ACRefsCounters>();
			if (refs)
			{
				if (priority != LoadingPriority_None && m_LoadingPriority == LoadingPriority_None)
					refs->LiveHandles++;
				if (priority == LoadingPriority_None && m_LoadingPriority != LoadingPriority_None)
					refs->LiveHandles--;
			}
			m_LoadingPriority = priority;
		} // TO DO: add AssetLoadingPriority counting

		AssetObserver<tnAsset> Observe() const { return AssetObserver<tnAsset>(GetECSHandle()); }
		AssetUser    <tnAsset> Use()     const { return AssetUser    <tnAsset>(GetECSHandle()); }

	private:
		ECS_AssetHandle GetECSHandle() const { return ECS_AssetHandle(AssetManager::GetRegistry(), m_ID); };

		void Init()
		{
			if (!m_ID) return;
			if (m_LoadingPriority == LoadingPriority_None) return;
			auto refs = GetECSHandle().try_get<ACRefsCounters>();
			if (!refs) return;
			refs->LiveHandles++;
		}

		void Deinit()
		{
			if (!m_ID) return;
			if (m_LoadingPriority == LoadingPriority_None) return;
			auto refs = GetECSHandle().try_get<ACRefsCounters>();
			if (!refs) return;
			refs->LiveHandles--;

			m_ID = NullAssetID;
		}
	};
}