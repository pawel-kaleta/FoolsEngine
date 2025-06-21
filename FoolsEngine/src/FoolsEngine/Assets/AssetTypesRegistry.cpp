#include "FE_pch.h"
#include "AssetTypesRegistry.h"

#include "AssetTypes.h"

namespace fe
{
	AssetTypesRegistry* AssetTypesRegistry::s_Instance;

	void AssetTypesRegistry::RegisterAssetTypes()
	{
#define _REGISTER_ASSET_TYPE_CALL(x) RegisterAssetType<x>();
		FE_FOR_EACH(_REGISTER_ASSET_TYPE_CALL, FE_ASSET_TYPES_LIST);
	}
}