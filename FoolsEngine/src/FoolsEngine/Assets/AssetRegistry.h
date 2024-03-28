#pragma once

#include "FoolsEngine/Core/Core.h"
#include "Asset.h"

#include <unordered_map>

namespace fe
{
	class AssetSourceRegistry
	{
	public:
		static void Add(const AssetSource& assetSource) { return s_AssetSourceRegistry->IAdd(assetSource); }
		static bool Exist(const AssetSourceID& ID) { return s_AssetSourceRegistry->IExist(ID); }
		static AssetSource* Get(const AssetSourceID& ID) { return s_AssetSourceRegistry->IGet(ID); }

		static const std::vector<AssetSource>& GetAll() { return s_AssetSourceRegistry->m_AssetSources; }
	private:
		static AssetSourceRegistry* s_AssetSourceRegistry; 
		std::vector<AssetSource> m_AssetSources;
		std::unordered_map<AssetSourceID, AssetSource*> m_MapByID;
		std::unordered_map<std::filesystem::path, AssetSource*> m_MapByPath;

		void IAdd(const AssetSource& assetSource)
		{ 
			m_AssetSources.push_back(assetSource);
			AssetSource* ptr = &(m_AssetSources.back());
			auto& ID = assetSource.ID;
			FE_CORE_ASSERT(!IExist(ID), "AssetSource already in registry!");
			m_AssetSources[ID] = assetSource;
		}
		bool IExist(const AssetSourceID& ID) { return m_AssetSources.find(ID) != m_AssetSources.end(); }
		AssetSource* IGet(const AssetSourceID& ID)
		{
			FE_CORE_ASSERT(IExist(ID), "AssetSource not found");
			return m_AssetSources[ID];
		}
	};


}