#pragma once

#include "AssetManager.h"
#include "AssetInterface.h"
#include "FoolsEngine\Debug\Asserts.h"

namespace fe
{
	// AssetObserver<> and AssetUser<> are not using AssetConcept as a 'constraint',
	// because their argument is sometimes forward declared (incomplete type fails constraint verification)
	// instead there should be static_assert(AssetConcept<tnAsset>) for every tnAsset class in AssetTypes.h

	template <typename tnAsset>
	class AssetObserver final : public tnAsset::Observer
	{
	public:
		static_assert(std::is_base_of_v<Asset, tnAsset>, "This is not an asset!");

		AssetObserver() = delete;
		AssetObserver(const AssetObserver& other) = delete;
		AssetObserver(AssetObserver&& other) = delete;
		AssetObserver& operator=(const AssetObserver& other) = delete;
		AssetObserver& operator=(AssetObserver&& other) = delete;
		~AssetObserver()
		{
			if (!tnAsset::Observer::IsValid()) return;

			auto refs = AssetInterface::GetRefCounters();
			if (!refs) return;

			refs->ActiveObserversCount--;
		}
		//TODO: mutexes

		AssetObserver(AssetID assetID) :
			tnAsset::Observer(ECS_AssetHandle(AssetManager::GetRegistry(), assetID))
		{
			FE_CORE_ASSERT(assetID != NullAssetID, "Cannot create AssetObserver from NullAssetID");
			Init();
		}
		//AssetObserver(ECS_AssetHandle ECS_handle) :
		//	tnAsset::Observer(ECS_handle)
		//{
		//	Init();
		//};

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
			if (!tnAsset::Observer::IsValid()) return;
			FE_CORE_ASSERT(AssetInterface::Get<ACAssetType>().Type == tnAsset::GetTypeStatic(), "This is not asset of this type!");

			StackCheck();

			auto refs = AssetInterface::GetRefCounters();
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
		AssetUser(AssetUser&& other) = delete;
		AssetUser& operator=(const AssetUser& other) = delete;
		AssetUser& operator=(AssetUser&& other) = delete;
		~AssetUser()
		{
			if (!tnAsset::User::IsValid()) return;
			auto refs = AssetInterface::GetRefCounters();
			if (!refs) return;
			refs->ActiveUser = false;
		}
		//TODO: mutexes

		AssetUser(AssetID assetID) :
			tnAsset::User(ECS_AssetHandle(AssetManager::GetRegistry(), assetID))
		{
			FE_CORE_ASSERT(assetID != NullAssetID, "Cannot create AssetUser from NullAssetID");
			Init();
		}
		
		void FlagLoaded()				{ this->Flag<ACLoaded>(); }
		void FlagLoadedAsDependency()	{ this->Flag<ACLoadedAsDependence>(); }
		void FlagUnloaded()				{ this->UnFlag<ACLoaded>(); }
		void ReleaseDependencyLoad()	{ this->UnFlag<ACLoadedAsDependence>(); }

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
			if (!tnAsset::User::IsValid()) return;
			FE_CORE_ASSERT(AssetInterface::Get<ACAssetType>().Type == tnAsset::GetTypeStatic(), "This is not asset of this type!");

			StackCheck();

			auto refs = AssetInterface::GetRefCounters();
			if (!refs) return;

			FE_CORE_ASSERT(!refs->ActiveObserversCount, "Cannot read and write at the same time");
			FE_CORE_ASSERT(!refs->ActiveUser, "Cannot write concurently");
			refs->ActiveUser = true; //TODO: mutexes
		}
	};
}