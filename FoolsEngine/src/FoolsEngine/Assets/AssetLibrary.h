#pragma once

#include "AssetHandle.h"
#include "FoolsEngine/Core/Core.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\Texture.h"
#include "Registries.h"

namespace fe
{
	class AssetLibrary
	{
	public:
		static void Add(Asset* assetPtr)
		{
			//AssetHandleTracker tracker(assetPtr);
			auto ID = assetPtr->GetSignature()->ID;
			if (IsLoaded(ID))
			{
				FE_CORE_ASSERT(false, "Asset already in library");
				return;
			}

			AssetType type = assetPtr->GetSignature()->Type;

			s_Maps[type][ID] = AssetHandleTracker(assetPtr);
		}

		/*
		static void Add(const AssetHandleTracker& assetTracker)
		{
			auto ID = assetTracker.GetAssetPtr()->GetSignature()->ID;
			if (IsLoaded(ID))
			{
				FE_CORE_ASSERT(false, "Asset already in library");
				return;
			}
			
			AssetType type = assetTracker.GetAssetPtr()->GetSignature()->Type;

			s_Maps[type][ID] = assetTracker;
		}
		*/

		static bool IsLoaded(AssetID assetID)
		{
			auto type = AssetSignatureRegistry::Get(assetID).Type;
			return s_Maps[type].find(assetID) != s_Maps[type].end();
		}

		template <typename tAsset>
		static AssetHandle<tAsset> GetAssetHandle(AssetID assetID)
		{
			static_assert(std::is_base_of<Asset, tAsset>::value);

			if (!IsLoaded(assetID))
			{
				FE_CORE_ASSERT(false, "Asset not found");
				return AssetHandle<tAsset>(nullptr, nullptr);
			}

			return s_Maps[tAsset::GetAssetType()].at(assetID).GetHandle<tAsset>();
		}

		template <typename tAsset>
		static std::unordered_map<AssetID, AssetHandleTracker>& GetAll()
		{
			static_assert(std::is_base_of<Asset, tAsset>::value);
			return s_Maps[tAsset::GetAssetType()];
		}

		static std::unordered_map<AssetID, AssetHandleTracker>& GetAll(AssetType type)
		{
			return s_Maps[type];
		}
	private:
		static std::unordered_map<AssetID, AssetHandleTracker> s_Maps[AssetType::TypesCount];
	};
}