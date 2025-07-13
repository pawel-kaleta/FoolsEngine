#pragma once

#include "AssetAccessors.h"

namespace fe
{
	class AssetHandleBase
	{
	public:
		AssetID GetID() const { return m_ID; }
		AssetLoadingPriority GetLoadingPriority() const { return m_LoadingPriority; }
	protected:
		AssetHandleBase() :
			m_ID(NullAssetID), m_LoadingPriority(AssetLoadingPriority::None) { };
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

		static AssetType GetTypeStatic() { return tnAsset::GetTypeStatic(); }

		AssetHandle() = default;
		AssetHandle(AssetID assetID, AssetLoadingPriority priority = AssetLoadingPriority::None) :
			AssetHandleBase(assetID, priority)
		{
			Init();
		};

		~AssetHandle()
		{
			Deinit();
		}

		AssetHandle(const AssetHandle& other, AssetLoadingPriority priority = AssetLoadingPriority::None) :
			AssetHandleBase(other.m_ID, priority)
		{
			Init();
		};
		AssetHandle(AssetHandle&& other) noexcept :
			AssetHandleBase(other.m_ID, other.m_LoadingPriority)
		{
			other.m_ID = NullAssetID;
		};
		AssetHandle& operator=(const AssetHandle& other)
		{
			Deinit();
		
			m_ID = other.m_ID;
		
			Init();
		
			return *this;
		}
		AssetHandle& operator=(AssetHandle&& other) noexcept
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
			if (priority == m_LoadingPriority) return;

			Deinit();
			m_LoadingPriority = priority;
			Init();
		}
		void SetID(AssetID assetID)
		{
			if (m_ID == assetID) return;

			Deinit();
			m_ID = assetID;
			Init();
		}

		AssetObserver<tnAsset> Observe() const { return AssetObserver<tnAsset>(GetECSHandle()); }
		AssetUser    <tnAsset> Use()     const { return AssetUser    <tnAsset>(GetECSHandle()); }

	private:
		ECS_AssetHandle GetECSHandle() const { return ECS_AssetHandle(AssetManager::GetRegistry(), m_ID); };

		void Init()
		{
			if (!m_ID) return;
			if (m_LoadingPriority == AssetLoadingPriority::None) return;
			auto ECSHandle = GetECSHandle();
			auto refs = ECSHandle.try_get<ACRefsCounters>();
			if (!refs) return;
			if (refs->LiveHandles[m_LoadingPriority].fetch_add(1) == 0)
			{
				switch (m_LoadingPriority)
				{
				case AssetLoadingPriority::Minimal:		ECSHandle.emplace<ACLoadFlag<AssetLoadingPriority::Minimal	>>(); break;
				case AssetLoadingPriority::VeryLow:		ECSHandle.emplace<ACLoadFlag<AssetLoadingPriority::VeryLow	>>(); break;
				case AssetLoadingPriority::Low:			ECSHandle.emplace<ACLoadFlag<AssetLoadingPriority::Low		>>(); break;
				case AssetLoadingPriority::Standard:	ECSHandle.emplace<ACLoadFlag<AssetLoadingPriority::Standard	>>(); break;
				case AssetLoadingPriority::High:		ECSHandle.emplace<ACLoadFlag<AssetLoadingPriority::High		>>(); break;
				case AssetLoadingPriority::VeryHigh:	ECSHandle.emplace<ACLoadFlag<AssetLoadingPriority::VeryHigh	>>(); break;
				case AssetLoadingPriority::Critical:	ECSHandle.emplace<ACLoadFlag<AssetLoadingPriority::Critical	>>(); break;
				}
			}
		}

		void Deinit()
		{
			if (!m_ID) return;
			if (m_LoadingPriority == AssetLoadingPriority::None) return;
			auto ECSHandle = GetECSHandle();
			auto refs = ECSHandle.try_get<ACRefsCounters>();
			if (!refs) return;
			if (refs->LiveHandles[m_LoadingPriority].fetch_sub(1) == 1)
			{
				switch (m_LoadingPriority)
				{
				case AssetLoadingPriority::Minimal:		ECSHandle.erase<ACLoadFlag<AssetLoadingPriority::Minimal	>>(); break;
				case AssetLoadingPriority::VeryLow:		ECSHandle.erase<ACLoadFlag<AssetLoadingPriority::VeryLow	>>(); break;
				case AssetLoadingPriority::Low:			ECSHandle.erase<ACLoadFlag<AssetLoadingPriority::Low		>>(); break;
				case AssetLoadingPriority::Standard:	ECSHandle.erase<ACLoadFlag<AssetLoadingPriority::Standard	>>(); break;
				case AssetLoadingPriority::High:		ECSHandle.erase<ACLoadFlag<AssetLoadingPriority::High		>>(); break;
				case AssetLoadingPriority::VeryHigh:	ECSHandle.erase<ACLoadFlag<AssetLoadingPriority::VeryHigh	>>(); break;
				case AssetLoadingPriority::Critical:	ECSHandle.erase<ACLoadFlag<AssetLoadingPriority::Critical	>>(); break;
				}
			}

		}
	};
}