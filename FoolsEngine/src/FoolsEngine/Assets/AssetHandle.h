#pragma once

#include "AssetManager.h"

#include "FoolsEngine\Debug\Asserts.h"

namespace fe
{
	template <typename tnAsset>
	class AssetObserver final : public tnAsset::Observer
	{
	public:
		static_assert(std::is_base_of_v<Asset, tnAsset>, "This is not an asset!");

		AssetObserver() = delete;
		AssetObserver(const AssetObserver& other) = delete;
		AssetObserver(AssetObserver&& other)      = delete;
		AssetObserver& operator=(const AssetObserver& other) = delete;
		AssetObserver& operator=(AssetObserver&& other)      = delete;
		~AssetObserver()
		{
			if (!IsValid()) return;

			auto refs = GetRefCounters();
			if (!refs) return;

			refs->ActiveObserversCount--;
		}
		//TODO: mutexes

		AssetObserver(AssetID assetID) :
			tnAsset::Observer(ECS_AssetHandle(AssetManager::GetRegistry(), assetID))
		{
			Init();
		}
		AssetObserver(ECS_AssetHandle ECS_handle) :
			tnAsset::Observer(ECS_handle)
		{
			Init();
		};

		static constexpr AssetType GetTypeStatic() { return tnAsset::GetTypeStatic(); }

	private:
		void StackCheck()
		{
#ifdef FE_INTERNAL_BUILD
			char dummy;
			ptrdiff_t displacement = &dummy - reinterpret_cast<char*>(this);
			FE_CORE_ASSERT(-10000 < displacement && displacement < 10000, "Don't put this on the heap!");
#endif // FE_INTERNAL_BUILD
		}

		void Init()
		{
			FE_CORE_ASSERT(Get<ACAssetType>().Type == tnAsset::GetTypeStatic(), "This is not asset of this type!");
			if (!IsValid()) return;

			StackCheck();

			auto refs = GetRefCounters();
			if (!refs) return; // internal assets are not reference counted

			FE_CORE_ASSERT(!refs->ActiveUser, "Cannot read and write at the same time");
			refs->ActiveObserversCount++; //TODO: mutexes
		}
	};

	template <typename tnAsset>
	class AssetUser final : public tnAsset::User
	{
	public:
		static_assert(std::is_base_of_v<Asset, tnAsset>, "This is not an asset!");

		AssetUser() = delete;
		AssetUser(const AssetUser& other) = delete;
		AssetUser(AssetUser&& other)      = delete;
		AssetUser& operator=(const AssetUser& other) = delete;
		AssetUser& operator=(AssetUser&& other)      = delete;
		~AssetUser()
		{
			if (!IsValid()) return;
			auto refs = GetRefCounters();
			if (!refs) return;
			refs->ActiveUser = false;
		}
		//TODO: mutexes

		AssetUser(AssetID assetID) :
			tnAsset::User(ECS_AssetHandle(AssetManager::GetRegistry(), assetID))
		{
			Init();
		}
		AssetUser(ECS_AssetHandle ECS_handle) :
			tnAsset::User(ECS_handle)
		{
			Init();
		};

		static constexpr AssetType GetTypeStatic() { return tnAsset::GetTypeStatic(); }

	private:
		void StackCheck()
		{
#ifdef FE_INTERNAL_BUILD
			char dummy;
			ptrdiff_t displacement = &dummy - reinterpret_cast<char*>(this);
			FE_CORE_ASSERT(-10000 < displacement && displacement < 10000, "Don't put this on the heap!");
#endif // FE_INTERNAL_BUILD
		}

		void Init()
		{
			if (!IsValid()) return;
			FE_CORE_ASSERT(Get<ACAssetType>().Type == tnAsset::GetTypeStatic(), "This is not asset of this type!");

			StackCheck();

			auto refs = GetRefCounters();
			if (!refs) return;

			FE_CORE_ASSERT(!refs->ActiveObserversCount, "Cannot read and write at the same time");
			FE_CORE_ASSERT(!refs->ActiveUser, "Cannot write concurently");
			refs->ActiveUser = true; //TODO: mutexes
		}
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