#pragma once

#include "FoolsEngine/Assets/AssetAccessors.h"

#include <filesystem>

namespace fe
{
	namespace AssetSerializer
	{
		void SerializeRegistry();
		bool DeserializeRegistry();

		template<typename tnAsset>
		void SerializeAsset(const AssetObserver<tnAsset>& assetObserver)
		{
			tnAsset::Serialize(assetObserver);
		}

		void LoadMetaData();

	};
}