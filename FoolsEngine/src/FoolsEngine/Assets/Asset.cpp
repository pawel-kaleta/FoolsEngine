#include "FE_pch.h"
#include "Asset.h"

#include "AssetManager.h"

namespace fe
{
	Asset::Asset(AssetType type, AssetID assetID) :
		m_ECSHandle(ECS_AssetHandle(*AssetManager::GetRegistry(type), assetID))
	{ }

	uint32_t BaseAssets::Counts(AssetType type)
	{
		static uint32_t Counts[AssetType::Count] = {
			(uint32_t)Scenes::Count,
			(uint32_t)Textures::Count,
			(uint32_t)Textures2D::Count,
			(uint32_t)Meshes::Count,
			(uint32_t)Models::Count,
			(uint32_t)Shaders::Count,
			(uint32_t)Materials::Count,
			(uint32_t)MaterialInstances::Count,
			(uint32_t)Audio::Count
		};

		return Counts[type];
	}

	bool BaseAssets::IsBaseAsset(AssetID id, AssetType type)
	{
		return id < Counts(type);
	}

}