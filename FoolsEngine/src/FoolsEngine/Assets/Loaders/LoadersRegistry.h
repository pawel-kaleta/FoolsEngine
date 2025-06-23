#pragma once

#include "LoaderType.h"
#include "FoolsEngine\Assets\AssetTypes.h"

namespace fe
{
	class LoadersRegistry
	{
	public:
		struct Item
		{
			bool (* const IsKnownExtensionFunkPtr)(const std::pmr::string&);
			bool (* const IsKnownAssetType)(AssetType assetType);
			const char const* SourceExtensionAlias;
			const LoaderType Type;
			// AssetType::None means multiple types 
			const AssetType AssetType;
		};

		static const std::vector<Item> GetItems() { return s_Instance->m_Items; }
		static const Item* GetItem(const std::pmr::string& extension);
		static const Item* GetItem(AssetType assetType);
	private:
		friend class Application;
		LoadersRegistry() { s_Instance = this; };
		void RegisterLoaders();
		void Shutdown() {};

		static LoadersRegistry* s_Instance;

		std::vector<Item> m_Items;

		template <typename tnLoader>
		static void RegisterLoader()
		{
			s_Instance->m_Items.push_back(
				Item{
					& tnLoader::IsKnownExtension,
					& tnLoader::IsKnownAssetType,
					tnLoader::GetExtensionAlias(),
					tnLoader::GetTypeStatic(),
					tnLoader::GetAssetTypeStatic()
				}
			);
		}
	};
}