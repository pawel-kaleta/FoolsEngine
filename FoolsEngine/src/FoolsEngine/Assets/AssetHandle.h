#pragma once

#include "AssetManager.h"

#include <memory>
#include <mutex>

namespace fe
{
	template <typename tAssetBody>
	class AssetHandle
	{
	public:
		AssetHandle() = default;
		AssetHandle(AssetID assetID) : m_AssetID(assetID) { }
		~AssetHandle()
		{
			if (m_Active)
			{
				GetRegistry()->get<ACSignature>(m_AssetID).m_ActiveHandleCount--;
			}
		}

		// the following are here to prevent spreading m_Active
		AssetHandle(const AssetHandle<tAssetBody>& other) : m_AssetID(other.m_AssetID) {}
		AssetHandle(AssetHandle<tAssetBody>&& other)      : m_AssetID(other.m_AssetID) {}
		AssetHandle<tAssetBody>& operator=(const AssetHandle<tAssetBody>& other)
		{
			m_AssetID  = other.m_AssetID;

			return *this;
		}
		AssetHandle<tAssetBody>& operator=(AssetHandle<tAssetBody>&& other)
		{
			m_AssetID  = other.m_AssetID;

			return *this;
		}

		bool IsValid() { return m_AssetID != NullAssetID; }
		bool IsActive() { return m_Active; }

		void Activate()
		{
			FE_CORE_ASSERT(IsValid(), "Cannot activate invalid AssetHandle!");

			GetRegistry()->get<ACSignature>(m_AssetID).m_ActiveHandleCount++;
			m_Active = true;
		}
	private:
		constexpr static AssetRegistry const* GetRegistry() { return AssetManager::GetRegistry<tAssetBody>(); }

		AssetID m_AssetID = NullAssetID;
		bool    m_Active  = false;
	};
}