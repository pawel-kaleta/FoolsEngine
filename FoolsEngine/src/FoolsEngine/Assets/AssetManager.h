#pragma once

#include "Asset.h"

namespace fe
{
	template <typename tAsset>
	class AssetHandle;

	class AssetManager
	{
	public:
		template <typename tAsset>
		static AssetHandle<tAsset> GetAssetHandle(AssetID assetID);
	private:
		AssetManager();
		static void UnloadAsset(AssetID assetID);

	};
}