#pragma once

#include "Asset.h"
#include "AssetManager.h"

#include <memory>
#include <mutex>

namespace fe
{
	// TO DO: mutex

	template <typename tAsset>
	class AssetHandle
	{
	public:
		AssetHandle() = delete;
		~AssetHandle()
		{
			(*m_RefCount)--;

			if (*m_RefCount == 1)
			{
				AssetManager::UnloadAsset(m_AssetPtr->GetID());
			}
			// AssetManager owns first instance and is responsible for releasing memory: Asset, RefCount 
		}

		AssetHandle(const AssetHandle<tAsset>& other)
			: m_ID(other.m_ID), m_AssetPtr(other.m_AssetPtr), m_RefCount(other.m_RefCount)
		{
			(*m_RefCount)++;
		}

		AssetHandle(AssetHandle<tAsset>&& other)
			: m_ID(other.m_ID), m_AssetPtr(other.m_AssetPtr), m_RefCount(other.m_RefCount)
		{
			other.m_ID = NULL;
			other.m_AssetPtr = nullptr;
			other.m_RefCount = nullptr;
			other.m_Mutex = nullptr;
		}

		AssetHandle<tAsset>& operator=(const AssetHandle<tAsset>& other)
			: m_ID(other.m_ID), m_AssetPtr(other.m_AssetPtr), m_RefCount(other.m_RefCount)
		{
			(*m_RefCount)++;

			return *this;
		}

		AssetHandle<tAsset>& operator=(AssetHandle<tAsset>&& other)
			: m_ID(other.m_ID), m_AssetPtr(other.m_AssetPtr), m_RefCount(other.m_RefCount)
		{
			other.m_ID = NULL;
			other.m_AssetPtr = nullptr;
			other.m_RefCount = nullptr;
			other.m_Mutex = nullptr;

			return *this;
		}

		tAsset& operator*() const { return *m_AssetPtr; }
		tAsset* operator->() const { return m_AssetPtr; }

	private:
		friend class AssetManager;
		AssetHandle(tAsset* asset) : m_AssetPtr(asset), m_ID(asset->GetID()), m_RefCount(new int(1)) { static_assert(std::is_base_of<Asset, tAsset>::value); }

		tAsset* m_AssetPtr = nullptr;
		std::atomic<int>* m_RefCount = nullptr;
	};
}