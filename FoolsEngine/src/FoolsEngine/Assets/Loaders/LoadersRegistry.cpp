#include "FE_pch.h"
#include "LoadersRegistry.h"

#include "TextureLoader.h"
#include "GeometryLoader.h"
#include "ShaderLoader.h"

namespace fe
{
	LoadersRegistry* LoadersRegistry::s_Instance;

	const LoadersRegistry::Item* LoadersRegistry::GetItem(const std::pmr::string& extension)
	{
		for (const auto& item : s_Instance->m_Items)
		{
			if ((*item.IsKnownExtensionFunkPtr)(extension))
				return &item;
		}
		return nullptr;
	}

	const LoadersRegistry::Item* LoadersRegistry::GetItem(AssetType assetType)
	{
		if (assetType == AssetType::None) return nullptr;

		for (const auto& item : s_Instance->m_Items)
		{
			if (item.AssetType == AssetType::None)
				if ((*item.IsKnownAssetType)(assetType))
					return &item;
			if (item.AssetType == assetType)
				return &item;
		}
	}

	void LoadersRegistry::RegisterLoaders()
	{
		RegisterLoader<TextureLoader>();
		RegisterLoader<GeometryLoader>();
		RegisterLoader<ShaderLoader>();
	}
}