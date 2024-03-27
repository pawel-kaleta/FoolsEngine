#pragma once

#include "FoolsEngine/Core/UUID.h"

#include <filesystem>

namespace fe
{
	using AssetID = UUID;

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

	class AssetSource
	{
	public:

	private:
		std::filesystem::path m_FilePath;
		UUID m_UUID;
	};

	class AssetProxy
	{
	public:
		UUID GetUUID() { return m_UUID; }
		AssetID GetAssetID() { return m_AssetID; }
		AssetType GetType() { return m_AssetType; }
		UUID GetAssetSourceID() { return m_AssetSourceID; }
	private:
		std::filesystem::path m_FilePath;
		UUID m_UUID;

		AssetID m_AssetID;
		AssetType m_AssetType = AssetType::None;
		UUID m_AssetSourceID;
	};
}