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
			// AssetHandleTracker owns RefCount (and asset?)
		}

		AssetHandle(const AssetHandle<tAsset>& other)
			: m_AssetPtr(other.m_AssetPtr), m_RefCount(other.m_RefCount)
		{
			(*m_RefCount)++;
		}

		AssetHandle(AssetHandle<tAsset>&& other)
			: m_AssetPtr(other.m_AssetPtr), m_RefCount(other.m_RefCount)
		{
			other.m_AssetPtr = nullptr;
			other.m_RefCount = nullptr;
		}

		AssetHandle<tAsset>& operator=(const AssetHandle<tAsset>& other)
			: m_AssetPtr(other.m_AssetPtr), m_RefCount(other.m_RefCount)
		{
			(*m_RefCount)++;

			return *this;
		}

		AssetHandle<tAsset>& operator=(AssetHandle<tAsset>&& other)
			: m_AssetPtr(other.m_AssetPtr), m_RefCount(other.m_RefCount)
		{
			other.m_AssetPtr = nullptr;
			other.m_RefCount = nullptr;

			return *this;
		}

		tAsset& operator*() const { return *m_AssetPtr; }
		tAsset* operator->() const { return m_AssetPtr; }

	private:
		friend class AssetHandleTracker<tAsset>;
		AssetHandle(tAsset* asset, std::atomic<int>* refCount) : m_AssetPtr(asset), m_RefCount() { static_assert(std::is_base_of<Asset, tAsset>::value); }

		tAsset* m_AssetPtr = nullptr;
		std::atomic<int>* m_RefCount = nullptr;
	};

	template <typename tAsset>
	class AssetHandleTracker
	{
	public:
		AssetHandleTracker(tAsset* asset)
			: m_Asset(asset)
		{
			m_RefCount = new std::atomic<int>(0);
		}
	private:
		tAsset* m_Asset = nullptr;
		std::atomic<int>* m_RefCount = nullptr;
	};
}