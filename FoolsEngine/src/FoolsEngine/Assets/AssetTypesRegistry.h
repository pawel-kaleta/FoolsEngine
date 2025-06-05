#pragma once

#include "Asset.h"

namespace fe
{
	class AssetTypesRegistry
	{
	public:
		struct Item
		{
			void (* EmplaceCore)(AssetID);
			bool (* LoadMetadata)(AssetID); //???
			// load data too? (for AssetManager::EvaluateAndReload)
			std::string TypeName;
			AssetType Type;
		};

		static const std::vector<Item> GetItems() { return s_Instance->m_Items; }

	private:
		friend class Application;
		AssetTypesRegistry() { s_Instance = this; };
		void RegisterAssetTypes();
		void Shutdown() {};

		static AssetTypesRegistry* s_Instance;

		std::vector<Item> m_Items;

		template <typename tnAsset>
		static void RegisterAssetType()
		{
			s_Instance->m_Items.push_back(
				Item{
					&tnAsset::EmplaceCore,
					&tnAsset::Deserialize,
					tnAsset::GetTypeStatic().ToString(),
					tnAsset::GetTypeStatic()
				}
			);
		}
	};
}