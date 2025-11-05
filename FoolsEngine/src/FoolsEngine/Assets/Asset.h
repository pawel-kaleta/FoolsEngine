#pragma once

#include "FoolsEngine/Core/UUID.h"

#include "FoolsEngine\Utils\DeclareEnum.h"

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

	FE_DECLARE_ENUM(AssetType,
		Texture,
		Texture2D,
	
		Shader,
		ShadingModel,
		Material,
	
		Mesh,
		RenderMesh,
		Model,
	
		Skeleton,
		SkinnedModel,
		Animation,
	
		Scene,
		Prefab,
	
		Audio,
	
		Count,
		None
	);

	class Asset
	{
	public:
		Asset() = delete;
	};

	struct AssetComponent { };

	struct AssetFlagComponent : AssetComponent { };

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

	FE_DECLARE_ENUM(AssetLoadingPriority,
		None,
		Minimal, VeryLow, Low, Standard, High, VeryHigh, Critical,
		Count
	);

	template <AssetLoadingPriority::ValueType loadingPriority>
	struct ACLoadFlag final : AssetFlagComponent {};
	struct ACLoaded final : AssetFlagComponent {};
	struct ACLoadedAsDependence final : AssetFlagComponent {};

	struct ACRefsCounters final : AssetComponent
	{
		// LiveHandles[ AssetLoadingPriority::None ]
		// is used to count how many assets keep this asset loaded as dependence (eg shading model loaded for multiple materials)
		// so the first can load and last unload this asset
		std::atomic<int> LiveHandles[AssetLoadingPriority::Count] = {};

		std::atomic<int> ActiveObserversCount = 0;
		bool ActiveUser = false; //TODO: make this a mutex and add all other control block code
	};
}