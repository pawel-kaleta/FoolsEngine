#pragma once

#include "FoolsEngine/Core/UUID.h"

#include <filesystem>

#include <yaml-cpp\yaml.h>

namespace fe
{
	using AssetID       = UUID;
	using AssetSourceID = UUID;
	using AssetProxyID  = UUID;

	enum AssetType
	{
		SceneAsset,
		TextureAsset,
		MeshAsset,
		ShaderAsset,
		MaterialAsset,
		MaterialInstanceAsset,
		Audio,

		TypesCount,
		None
	};

	struct AssetSignature
	{
		AssetType Type = AssetType::None;
		AssetID ID;
		AssetProxyID ProxyID = 0;       //
		std::filesystem::path FilePath; // set only for non-virtualized non-runtime only assets (existing on disk)
	};

	struct AssetSource
	{
		std::filesystem::path FilePath;
		AssetSourceID ID;
		
		std::vector<AssetProxyID> AssetProxies;
	};

	struct AssetProxy
	{
		std::filesystem::path FilePath;
		AssetProxyID ID;

		AssetID AssetID = 0;
		AssetType AssetType = AssetType::None;
		AssetSourceID AssetSourceID = 0;
		void* ImportSettings = nullptr;
	};

	class Asset
	{
	public:
		Asset() = default;
		Asset(AssetSignature* signature) : m_Signature(signature) { }
		AssetSignature* GetSignature() const { return m_Signature; }

		static AssetType GetAssetType() { return AssetType::None; }
	protected:
		AssetSignature* m_Signature = nullptr;
	};
}