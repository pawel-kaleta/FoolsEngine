#pragma once

#include "AssetManager.h"

#include <memory>

namespace fe
{
	template <typename tAssetBody> class AssetHandle
	{
	public:
		AssetHandle() = default;
		AssetHandle(AssetID assetID) : m_AssetID(assetID) { }
		~AssetHandle() { if (m_Active) GetRegistry()->get<ACSignature>(m_AssetID).m_ActiveHandleCount--; }

		// the following are here to prevent spreading m_Active
		AssetHandle(const AssetHandle<tAssetBody>& other) : m_AssetID(other.m_AssetID) { }
		AssetHandle(AssetHandle<tAssetBody>&& other) : m_AssetID(other.m_AssetID) { }
		AssetHandle<tAssetBody>& operator=(const AssetHandle<tAssetBody>& other)
		{
			m_AssetID = other.m_AssetID;

			return *this;
		}
		AssetHandle<tAssetBody>& operator=(AssetHandle<tAssetBody>&& other)
		{
			m_AssetID = other.m_AssetID;

			return *this;
		}

		bool IsValid() { return NullAssetID != m_AssetID; }
		bool IsActive() { return m_Active; }

		void Activate()
		{
			FE_CORE_ASSERT(IsValid(), "Cannot activate invalid AssetHandle!");
			FE_CORE_ASSERT(!m_Active, "Cannot activate active AssetHandle!");

			GetRegistry()->get<ACSignature>(m_AssetID).m_ActiveHandleCount++;
			m_Active = true;
		}
		void Deactivate()
		{
			FE_CORE_ASSERT(IsValid(), "Cannot deactivate invalid AssetHandle!");
			FE_CORE_ASSERT(m_Active, "Cannot deactivate inactive AssetHandle!");

			GetRegistry()->get<ACSignature>(m_AssetID).m_ActiveHandleCount--;
			m_Active = false;
		}

		void Reset(AssetID assetID)
		{
			if (m_Active)
			{
				GetRegistry()->get<ACSignature>(m_AssetID).m_ActiveHandleCount--;
				m_Active = false;
			}
			m_AssetID = assetID;
		}
		void Reset() { Reset(NullAssetID); }

		template <typename tAC>
		tAC* Get()
		{
			FE_CORE_ASSERT(IsValid(), "AssetHandle is not valid");
			return GetRegistry()->try_get<tAC>(m_AssetID);
		}

		tAssetBody* AssetBody() { return Get<tAssetBody>(); }
		ACSignature* Signature() { return Get<ACSignature>(); }
		ACFilepath* Filepath() { return Get<ACFilepath>(); }
		ACAssetProxy* AssetProxy() { return Get<ACAssetProxy>(); }
	private:
		constexpr static AssetRegistry* GetRegistry() { return AssetManager::GetRegistry<tAssetBody>(); }

		AssetID m_AssetID = NullAssetID;
		bool    m_Active = false;
	};
}