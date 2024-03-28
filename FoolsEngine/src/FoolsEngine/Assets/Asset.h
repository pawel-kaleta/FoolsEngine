#pragma once

#include "FoolsEngine/Core/UUID.h"

#include <filesystem>

namespace fe
{
	using AssetID = UUID;
	using AssetSourceID = UUID;
	using AssetProxyID = UUID;

	enum AssetType
	{
		None = 0,
		Scene,
		Texture2D,
		Mesh,
		Material,
		MaterialInstance,
		Substance
	};

	class Asset
	{
	public:
		Asset() = delete;
		Asset(AssetType type) : m_Type(type) { }

		AssetID GetID() { return m_ID; }
		AssetType GetType() { return m_Type; }
	private:
		AssetType m_Type = AssetType::None;
		AssetID m_ID;
	};


	struct AssetSource
	{
		std::filesystem::path FilePath;
		AssetSourceID ID;
	};


	class AssetProxy
	{
	public:
		AssetProxyID GetID() { return m_ID; }
		AssetID GetAssetID() { return m_AssetID; }
		AssetType GetType() { return m_AssetType; }
		AssetSourceID GetAssetSourceID() { return m_AssetSourceID; }
	private:
		std::filesystem::path m_FilePath;
		AssetProxyID m_ID;

		AssetID m_AssetID;
		AssetType m_AssetType = AssetType::None;
		AssetProxyID m_AssetSourceID;
	};
}