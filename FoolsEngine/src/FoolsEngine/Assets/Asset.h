#pragma once

#include "FoolsEngine/Core/UUID.h"

#include <filesystem>
#include <entt/entity/registry.hpp>
#include <entt/entity/handle.hpp>
#include <entt/entity/helper.hpp>

namespace fe
{
	using AssetID = uint32_t;
	using AssetRegistry = entt::basic_registry<AssetID>;
	inline constexpr entt::null_t NullAssetID{};

	using ECS_AssetHandle = entt::basic_handle<AssetRegistry>;
	using Const_ECS_AssetHandle = entt::basic_handle<const AssetRegistry>;

	enum AssetType
	{
		TextureAsset,
		Texture2DAsset,

		ShaderAsset,
		ShadingModelAsset,
		MaterialAsset,

		MeshAsset,
		RenderMeshAsset,
		ModelAsset,

		SkeletonAsset,
		SkinnedModelAsset,
		AnimationAsset,
		
		SceneAsset,
		PrefabAsset,
		
		AudioAsset,

		Count,
		None
	};

	const char* AssetTypeName[AssetType::Count] = {
		"TextureAsset",
		"Texture2DAsset",

		"ShaderAsset",
		"ShadingModelAsset",
		"MaterialAsset",

		"MeshAsset",
		"RenderMeshAsset",
		"ModelAsset",

		"SkeletonAsset",
		"SkinnedModelAsset",
		"AnimationAsset",

		"SceneAsset",
		"PrefabAsset",

		"AudioAsset"
	};

	class Asset
	{
	public:
		Asset() = delete;
	};

	struct AssetComponent { };

	struct ACAssetType final : AssetComponent
	{
		AssetType Type;
	};

	struct ACUUID final : AssetComponent
	{
		UUID UUID;
	};

	struct ACMasterAsset final : AssetComponent
	{
		AssetID Master;
	};

	struct ACFilepath final : AssetComponent
	{
		std::filesystem::path Filepath;
	};

	struct ACSourceFilepath final : AssetComponent
	{
		std::filesystem::path Filepath;
	};

	struct ACRefsCounters final : AssetComponent
	{
		bool ActiveUser = false; //TODO: make this a mutex and add all other control block code
		std::atomic<int> ActiveObserversCount = 0;

		// TO DO: add AssetLoadingPriority counting
		std::atomic<int> LiveHandles = 0;

		// TO DO: split into 2 components ?
	};
}