#pragma once

#include "Asset.h"
#include "Loaders/LoaderType.h"
#include "FoolsEngine/Foundation/Containers/String.h"
#include "FoolsEngine/Foundation/Containers/Arena.h"


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
			CString TypeCString;
			AssetType Type;
		};

		static AssetTypesRegistry& Get() { return *s_Instance; }

		static const Item* GetItem(AssetType assetType);
		static const Item* GetItem(const std::pmr::string& extension);
		
		DynamicArena<Item> m_Items;
	private:
		static AssetTypesRegistry* s_Instance;

		friend class Application;
		
		AssetTypesRegistry()
		{
			s_Instance = this;
			m_Items.Init();
		};
		
		template <typename tnAsset>
		void RegisterAssetType();
		
		void RegisterAssetTypes();
		
		void Shutdown() {};
	};
}