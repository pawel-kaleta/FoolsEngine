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
			void (* LoadMetadata)(AssetID); //???
			// load data too? (for AssetManager::EvaluateAndReload)
			std::string TypeName;
			AssetType Type;
		};

		template <typename tnAsset>
		static void RegisterAssetType()
		{
			s_Instance->m_Items.push_back(
				Item{
					&tnAsset::EmplaceCore,
					nullptr, //??
					AssetTypeName[tnAsset::GetTypeStatic()],
					tnAsset::GetTypeStatic()
				}
			);
		}

		static const std::vector<const Item> GetItems() { return s_Instance->m_Items; }

	private:
		friend class Application;
		AssetTypesRegistry() { s_Instance = this; };
		void RegisterAssetTypes();
		void Shutdown() {};

		static AssetTypesRegistry* s_Instance;

		std::vector<const Item> m_Items;
	};
}