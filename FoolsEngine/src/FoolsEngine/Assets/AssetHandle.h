#pragma once

#include "Asset.h"
#include "AssetManager.h"

#include <memory>
#include <mutex>

namespace fe
{
	template <typename tAsset>
	class AssetHandle
	{
	public:
		AssetHandle() = default;
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
		{
			m_AssetPtr = other.m_AssetPtr;
			m_RefCount = other.m_RefCount;
			(*m_RefCount)++;

			return *this;
		}

		AssetHandle<tAsset>& operator=(AssetHandle<tAsset>&& other)
		{
			m_AssetPtr = other.m_AssetPtr;
			m_RefCount = other.m_RefCount;
			other.m_AssetPtr = nullptr;
			other.m_RefCount = nullptr;

			return *this;
		}

		tAsset& operator*() const { return *m_AssetPtr; }
		tAsset* operator->() const { return m_AssetPtr; }

		operator bool() const { return m_AssetPtr; }

		tAsset* Raw() const { return m_AssetPtr; }

	private:
		friend class AssetHandleTracker;
		AssetHandle(tAsset* asset, std::atomic<int>* refCount) : m_AssetPtr(asset), m_RefCount() { static_assert(std::is_base_of<Asset, tAsset>::value); }

		tAsset* m_AssetPtr = nullptr;
		std::atomic<int>* m_RefCount = nullptr;
	};

	class AssetHandleTracker
	{
	public:
		AssetHandleTracker(Asset* asset)
			: m_Asset(asset)
		{
			m_RefCount = new std::atomic<int>(0);
		}

		Asset* GetAssetPtr() const { return m_Asset; }

		template <typename tAsset>
		AssetHandle<tAsset> GetHandle()
		{
			(*m_RefCount)++;
			return AssetHandle<tAsset>(m_Asset, m_RefCount);
		}
	private:
		Asset* m_Asset = nullptr;
		std::atomic<int>* m_RefCount = nullptr;
	};
}