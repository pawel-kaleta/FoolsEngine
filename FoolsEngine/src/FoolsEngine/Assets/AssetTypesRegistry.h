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

		static AssetTypesRegistry& Get() { return *s_Instance; }

		static const Item* GetItem(AssetType assetType);
		static const Item* GetItem(const std::pmr::string extension);
		
		std::vector<Item> m_Items;
	private:
		static AssetTypesRegistry* s_Instance;

		friend class Application;
		
		AssetTypesRegistry() { s_Instance = this; };
		
		template <typename tnAsset>
		void RegisterAssetType();
		
		void RegisterAssetTypes();
		
		void Shutdown() {};
	};
}