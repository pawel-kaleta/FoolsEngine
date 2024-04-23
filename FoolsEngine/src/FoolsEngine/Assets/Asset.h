#pragma once

#include "FoolsEngine/Core/UUID.h"

#include <filesystem>

#include <yaml-cpp\yaml.h>

#include <memory>

#include <entt/entity/registry.hpp>
#include <entt/entity/handle.hpp>
#include <entt/entity/helper.hpp>

namespace fe
{
	using AssetID = uint32_t;
	inline constexpr entt::null_t NullAssetID{};

	using AssetSourceID = uint32_t;

	enum AssetType
	{
		SceneAsset,
		TextureAsset,
		Texture2DAsset,
		MeshAsset,
		ShaderAsset,
		MaterialAsset,
		MaterialInstanceAsset,
		AudioAsset,

		TypesCount,
		None
	};

	struct AssetComponent {};

	struct ACBody
	{
		constexpr static AssetType GetStaticType() { return AssetType::None; }
	};

	struct ACSignature : AssetComponent
	{
		UUID UUID;
		AssetType Type = AssetType::None;
		std::atomic<int> m_ActiveHandleCount = 0;
	};

	struct ACFilepath : AssetComponent
	{
		std::filesystem::path Filepath;
	};

	struct ACName : AssetComponent
	{
		std::string Name;
	};

	struct ACAssetProxy
	{
		std::filesystem::path FilePath;
		AssetSourceID AssetSourceID;

		// to do: separate component? dont hold and read from file upon request?
		void* ImportSettings = nullptr;
	};

	struct AssetSource
	{
		std::filesystem::path FilePath;
		UUID UUID;

		std::vector<AssetID> AssetIDs;
	};
}