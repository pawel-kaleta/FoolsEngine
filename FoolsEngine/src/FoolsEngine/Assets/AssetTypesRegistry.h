#pragma once

#include "Asset.h"
#include "Loaders\LoaderType.h"

namespace YAML { class Emitter; }

namespace fe
{
	class AssetTypesRegistry
	{
	public:
		struct Item
		{
			void (* EmplaceCore)(AssetID);
			bool (* LoadMetadata)(AssetID);
			void (* SaveMetadata)(YAML::Emitter&, AssetID);
			const char* MetaFileExtension;
			// load data too? (for AssetManager::EvaluateAndReload)
			const char* TypeConstCharPtr;
			AssetType Type;
		};

		static const std::vector<Item> GetItems() { return s_Instance->m_Items; }
		static const Item* GetItem(AssetType assetType);
		static const Item* GetItem(const std::pmr::string extension);
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
					&tnAsset::LoadMetadata,
					&tnAsset::SaveMetadata,
					tnAsset::GetMetaFileExtension(),
					tnAsset::GetTypeStatic().ToConstCharPtr(),
					tnAsset::GetTypeStatic()
				}
			);
		}
	};
}