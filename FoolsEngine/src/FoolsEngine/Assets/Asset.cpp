#include "FE_pch.h"
#include "Asset.h"

#include "AssetManager.h"

namespace fe
{
	Asset::Asset(AssetType type, AssetID assetID) :
		m_ECSHandle(ECS_AssetHandle(*AssetManager::GetRegistry(type), assetID))
	{ }

	void Asset::SetFilepath(const std::filesystem::path& path)
	{
		FE_CORE_ASSERT(!path.empty(), "Empty path!");
		FE_CORE_ASSERT(m_ECSHandle, "Invlaid ECS asset handle");
		
		const AssetType type = GetType();
		auto& ac_sourcepath = Get<ACDataFilepath>();

		ac_sourcepath.Filepath = path;
		AssetManager::RemoveByFilepathMapEntry(path, type);
		AssetManager::AddByFilepathMapEntry(path, GetID(), type);
	}
}