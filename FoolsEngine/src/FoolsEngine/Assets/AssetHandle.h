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
#ifdef FE_INTERNAL_BUILD
			char dummy;
			ptrdiff_t displacement = &dummy - reinterpret_cast<char*>(this);
			FE_CORE_ASSERT(-10000 < displacement && displacement < 10000, "Don't put this on the heap!");
#endif // FE_INTERNAL_BUILD

			if (IsValid())
			{
				GetRefCounters().ActiveObserversCount++; //TODO: mutexes
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
#ifdef FE_INTERNAL_BUILD
			char dummy;
			ptrdiff_t displacement = &dummy - reinterpret_cast<char*>(this);
			FE_CORE_ASSERT(-10000 < displacement && displacement < 10000, "Don't put this on the heap!");
#endif // FE_INTERNAL_BUILD

			if (IsValid())
			{
				GetRefCounters().ActiveUser = true; //TODO: mutexes
			}
		};
	};



	class AssetHandleBase
	{
	protected:
		AssetHandleBase() : m_ID(NullAssetID) {};
		AssetHandleBase(AssetID id) : m_ID(id) {};
		
		AssetID m_ID;
	};

	template <typename tnAsset>
	class AssetHandle : public AssetHandleBase
	{
	public:
		static_assert(std::is_base_of_v<Asset, tnAsset>, "This is not an asset!");

		static AssetType GetType() { return tnAsset::GetTypeStatic(); }

		AssetHandle() = default;
		AssetHandle(ECS_AssetHandle assetHandle) :
			AssetHandleBase(assetHandle.entity())
		{
			if (assetHandle)
				assetHandle.get<ACRefsCounters>().LiveHandles++;
		};
		AssetHandle(AssetID assetID) :
			AssetHandleBase(assetID)
		{
			auto ECShandle = GetECSHandle();
			if (ECShandle.valid())
				ECShandle.get<ACRefsCounters>().LiveHandles++;
		};

		~AssetHandle()
		{
			auto ECShandle = GetECSHandle();
			if (ECShandle.valid())
				ECShandle.get<ACRefsCounters>().LiveHandles--;
		}

		AssetHandle(const AssetHandle& other) :
			AssetHandleBase(other.m_ID)
		{
			auto ECShandle = GetECSHandle();
			if (ECShandle.valid())
				ECShandle.get<ACRefsCounters>().LiveHandles++;
		};
		AssetHandle(AssetHandle&& other) :
			AssetHandleBase(other.m_ID)
		{
			other.m_ID = NullAssetID;
		};
		AssetHandle& operator=(const AssetHandle& other)
		{
			m_ID = other.m_ID;
			auto ECShandle = GetECSHandle();
			if (ECShandle.valid())
				ECShandle.get<ACRefsCounters>().LiveHandles++;

			return *this;
		}
		AssetHandle& operator=(AssetHandle&& other)
		{
			m_ID = other.m_ID;
			other.m_ID = NullAssetID;

			return *this;
		}

		bool operator==(const AssetHandle& other) const { return m_ID == other.m_ID; }

		AssetID GetID() const { return m_ID; }
		UUID GetUUID() const { return GetECSHandle().get<ACUUID>().UUID; }

		bool IsValid() const { return (bool)GetECSHandle(); }

		const AssetObserver<tnAsset> Observe() const { return AssetObserver<tnAsset>(GetECSHandle()); }
		      AssetUser    <tnAsset> Use()     const { return AssetUser    <tnAsset>(GetECSHandle()); }

	private:
		ECS_AssetHandle GetECSHandle() const { return ECS_AssetHandle(*AssetManager::GetRegistry(GetType()), m_ID); };
	};
}