#pragma once

#include "FoolsEngine/Assets/AssetAccessors.h"

namespace fe
{
	namespace AssetSerializer
	{
		void SerializeRegistry();
		bool DeserializeRegistry();

		template<typename tnAsset>
		void SerializeAsset(const AssetObserver& assetObserver)
		{
			tnAsset::Serialize(assetObserver);
		}

		void LoadMetaData();

	};
}