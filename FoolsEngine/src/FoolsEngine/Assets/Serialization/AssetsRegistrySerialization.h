#pragma once

#include "FoolsEngine\Assets\AssetAccessors.h"

#include <filesystem>

namespace fe
{
	namespace AssetSerializer
	{
		void SerializeRegistry(const std::filesystem::path& filepath);
		bool DeserializeRegistry(const std::filesystem::path& filepath);

		template<typename tnAsset>
		void SerializeAsset(const AssetObserver<tnAsset>& assetObserver)
		{
			tnAsset::Serialize(assetObserver);
		}

		void LoadMetaData();

	};
}