#pragma once

#include "AssetManager.h"

namespace fe
{
	template <typename tnAsset>
	class AssetObserver final : public tnAsset
	{
	public:
		static_assert(std::is_base_of_v<Asset, tnAsset>, "This is not an asset!");

		AssetObserver() = delete;
		AssetObserver(const AssetObserver& other) = delete;
		AssetObserver(AssetObserver&& other)      = delete;
		AssetObserver& operator=(const AssetObserver& other) = delete;
		AssetObserver& operator=(AssetObserver&& other)      = delete;
		~AssetObserver() { if (IsValid()) GetRefCounters().ActiveObserversCount--; } //TODO: mutexes

		AssetObserver(ECS_AssetHandle ECS_handle) :
			tnAsset(ECS_handle)
		{
			FE_CORE_ASSERT(Get<ACAssetType>().Type == tnAsset::GetTypeStatic(), "This is not asset of this type!");

#ifdef FE_INTERNAL_BUILD
			char dummy;
			ptrdiff_t displacement = &dummy - reinterpret_cast<char*>(this);
			FE_CORE_ASSERT(-10000 < displacement && displacement < 10000, "Don't put this on the heap!");
#endif // FE_INTERNAL_BUILD

			if (IsValid())
			{
				ACRefsCounters& refs = GetRefCounters();
				FE_CORE_ASSERT(!refs.ActiveUser, "Cannot read and write at the same time");

				refs.ActiveObserversCount++; //TODO: mutexes
			}
		};
	};

	template <typename tnAsset>
	class AssetUser final : public tnAsset
	{
	public:
		static_assert(std::is_base_of_v<Asset, tnAsset>, "This is not an asset!");

		AssetUser() = delete;
		AssetUser(const AssetUser& other) = delete;
		AssetUser(AssetUser&& other)      = delete;
		AssetUser& operator=(const AssetUser& other) = delete;
		AssetUser& operator=(AssetUser&& other)      = delete;
		~AssetUser() { if (IsValid()) GetRefCounters().ActiveUser = false; } //TODO: mutexes

		AssetUser(ECS_AssetHandle ECS_handle) :
			tnAsset(ECS_handle)
		{
			FE_CORE_ASSERT(Get<ACAssetType>().Type == tnAsset::GetTypeStatic(), "This is not asset of this type!");

#ifdef FE_INTERNAL_BUILD
			char dummy;
			ptrdiff_t displacement = &dummy - reinterpret_cast<char*>(this);
			FE_CORE_ASSERT(-10000 < displacement && displacement < 10000, "Don't put this on the heap!");
#endif // FE_INTERNAL_BUILD

			if (IsValid())
			{
				ACRefsCounters& refs = GetRefCounters();
				FE_CORE_ASSERT(!refs.ActiveObserversCount, "Cannot read and write at the same time");
				FE_CORE_ASSERT(!refs.ActiveUser, "Cannot write concurently");
				refs.ActiveUser = true; //TODO: mutexes
			}
		};
	};


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
			if (assetID && priority != LoadingPriority_None)
				GetECSHandle().get<ACRefsCounters>().LiveHandles++;
		};

		~AssetHandle()
		{
			if (m_ID && m_LoadingPriority != LoadingPriority_None)
				GetECSHandle().get<ACRefsCounters>().LiveHandles--;
		}

		AssetHandle(const AssetHandle& other) :
			AssetHandleBase(other.m_ID, other.m_LoadingPriority)
		{
			if (other.m_ID && other.m_LoadingPriority != LoadingPriority_None)
				GetECSHandle().get<ACRefsCounters>().LiveHandles++;
		};
		AssetHandle(AssetHandle&& other) :
			AssetHandleBase(other.m_ID, other.m_LoadingPriority)
		{
			other.m_ID = NullAssetID;
		};
		AssetHandle& operator=(const AssetHandle& other)
		{
			if (m_ID && m_LoadingPriority != LoadingPriority_None)
				GetECSHandle().get<ACRefsCounters>().LiveHandles--;

			m_ID = other.m_ID;
			m_LoadingPriority = other.m_LoadingPriority;

			if (other.m_ID && other.m_LoadingPriority != LoadingPriority_None)
				GetECSHandle().get<ACRefsCounters>().LiveHandles++;

			return *this;
		}
		AssetHandle& operator=(AssetHandle&& other)
		{
			m_ID = other.m_ID;
			m_LoadingPriority = other.m_LoadingPriority;

			other.m_ID = NullAssetID;

			return *this;
		}

		bool operator==(const AssetHandle& other) const { return m_ID == other.m_ID; }
		
		UUID GetUUID() const { return GetECSHandle().get<ACUUID>().UUID; }
		bool IsValid() const { return (bool)GetECSHandle(); }
		void SetLoadingPriority(AssetLoadingPriority priority) { m_LoadingPriority = priority; } // TO DO: add AssetLoadingPriority counting

		const AssetObserver<tnAsset> Observe() const { return AssetObserver<tnAsset>(GetECSHandle()); }
		      AssetUser    <tnAsset> Use()     const { return AssetUser    <tnAsset>(GetECSHandle()); }

	private:
		ECS_AssetHandle GetECSHandle() const { return ECS_AssetHandle(AssetManager::GetRegistry(), m_ID); };
		
	};
}