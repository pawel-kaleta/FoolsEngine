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
		AssetProxyID ProxyID = 0;
	};

	struct AssetSource
	{
		std::filesystem::path FilePath;
		AssetSourceID ID;
		
		std::vector<AssetProxyID> AssetProxies;
	};

	class AssetProxy
	{
	public:
		AssetProxyID GetID() const { return m_ID; }
		AssetID GetAssetID() const { return m_AssetID; }
		AssetType GetAssetType() const { return m_AssetType; }
		AssetSourceID GetAssetSourceID() const { return m_AssetSourceID; }
	private:
		std::filesystem::path m_FilePath;
		AssetProxyID m_ID;

		AssetID m_AssetID;
		AssetType m_AssetType = AssetType::None;
		AssetSourceID m_AssetSourceID;
		void* m_ImportSettings = nullptr;
	};

	class Asset
	{
	public:
		Asset() = default;
		Asset(const AssetSignature& sygnature) : m_Signature(sygnature) { }
		const AssetSignature& GetSignature() const { return m_Signature; }
	protected:
		AssetSignature m_Signature;
	};
}