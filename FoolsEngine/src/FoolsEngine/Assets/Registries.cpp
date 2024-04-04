#include "FE_pch.h"
#include "Registries.h"

namespace fe
{
	AssetSourceRegistry::InternalData AssetSourceRegistry::s_Data;
	AssetProxyRegistry::InternalData  AssetProxyRegistry::s_Data;
	std::unordered_map<AssetID, AssetSignature> AssetSygnatureRegistry::s_MapByID;

	void AssetSourceRegistry::Add(const AssetSource& assetSource)
	{
		auto& ID = assetSource.ID;
		if (Exist(ID))
		{
			FE_CORE_ASSERT(false, "AssetSource already in registry!");
			return;
		}
		s_Data.m_AssetSources.push_back(assetSource);
		AssetSource* ptr = &(s_Data.m_AssetSources.back());
		s_Data.s_MapByID[ID] = ptr;
	}

	void AssetProxyRegistry::Add(const AssetProxy& assetProxy)
	{
		auto& ID = assetProxy.GetID();
		if (Exist(ID))
		{
			FE_CORE_ASSERT(false, "AssetProxy already in registry!");
			return;
		}
		s_Data.m_AssetProxies.push_back(assetProxy);
		AssetProxy* ptr = &(s_Data.m_AssetProxies.back());
		s_Data.s_MapByID[ID] = ptr;
	}
}