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
			bool (* LoadMetadata)(AssetID);
			void (* SaveMetadata)(AssetID);
			const char* (*GetMetaFileExtension)();
			// load data too? (for AssetManager::EvaluateAndReload)
			AssetType Type;
			const char* TypeConstCharPtr;
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
					&tnAsset::LoadMetadata,
					&tnAsset::SaveMetadata,
					&tnAsset::GetMetaFileExtension,
					tnAsset::GetTypeStatic(),
					tnAsset::GetTypeStatic().ToConstCharPtr()
				}
			);
		}
	};
}