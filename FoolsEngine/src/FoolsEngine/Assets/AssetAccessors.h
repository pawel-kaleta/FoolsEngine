#pragma once

#include "AssetManager.h"
#include "AssetInterface.h"

#include "FoolsEngine/Foundation/Debug/Asserts.h"

namespace fe
{
	class AssetObserver final : public AssetInterface
	{
	public:
		AssetObserver() = delete;
		AssetObserver(const AssetObserver& other) = delete;
		AssetObserver(AssetObserver&& other) = delete;
		AssetObserver& operator=(const AssetObserver& other) = delete;
		AssetObserver& operator=(AssetObserver&& other) = delete;
		~AssetObserver()
		{
			if (!IsValid()) return;

			auto refs = GetRefCounters();
			if (!refs) return;

			//TODO: mutex
		}

		AssetObserver(AssetID assetID) :
			AssetInterface(ECS_AssetHandle(AssetManager::Get().m_Registry, assetID))
		{
			Init();
		}

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

			StackCheck();

			auto refs = GetRefCounters();
			if (!refs) return; // internal assets are not reference counted

			FE_CORE_ASSERT(!refs->ActiveUser, "Cannot read and write at the same time");
			//TODO: mutex
		}
	};


	class AssetUser final : public AssetInterface
	{
	public:
		AssetUser() = delete;
		AssetUser(const AssetUser& other) = delete;
		AssetUser(AssetUser&& other) = delete;
		AssetUser& operator=(const AssetUser& other) = delete;
		AssetUser& operator=(AssetUser&& other) = delete;
		~AssetUser()
		{
			if (!IsValid()) return;
			auto refs = GetRefCounters();
			if (!refs) return;
			refs->ActiveUser = false;
		}
		//TODO: mutex

		AssetUser(AssetID assetID) :
			AssetInterface(ECS_AssetHandle(AssetManager::Get().m_Registry, assetID))
		{
			Init();
		}
		
		void FlagLoaded()				{ this->Flag<ACLoaded>(); }
		void FlagLoadedAsDependency()	{ this->Flag<ACLoadedAsDependence>(); }
		void FlagUnloaded()				{ this->UnFlag<ACLoaded>(); }
		void ReleaseDependencyLoad()	{ this->UnFlag<ACLoadedAsDependence>(); }

		bool IsLoaded()				{ return this->AllOf<ACLoaded>(); }
		bool IsLoadedAsDependency()	{ return this->AllOf<ACLoadedAsDependence>(); } // ?? check master?

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

			StackCheck();

			auto refs = AssetInterface::GetRefCounters();
			if (!refs) return;

			FE_CORE_ASSERT(!refs->ActiveUser, "Cannot write concurently");
			refs->ActiveUser = true; //TODO: mutex
		}
	};
}