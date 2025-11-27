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
			const char* SourceExtensionAlias;
			const LoaderType Type;
			// AssetType::None means multiple types? 
			const AssetType AssetType;
		};

		std::vector<Item> m_Items;

		static const Item* GetItem(const std::pmr::string& extension);
		static const Item* GetItem(AssetType assetType);
	private:
		static LoadersRegistry* s_Instance;

		friend class Application;
		LoadersRegistry() { s_Instance = this; };
		void RegisterLoaders();
		void Shutdown() {};

		template <typename tnLoader>
		void RegisterLoader();
	};
}