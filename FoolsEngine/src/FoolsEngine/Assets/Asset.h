#pragma once

#include "FoolsEngine/Foundation/Utils/DeclareEnum.h"

#include "FoolsEngine/Application/UUID.h"

#include "FoolsEngine/Renderer/2 - Resource/RStaticBuffer.h"

#include <entt/entity/registry.hpp>
#include <entt/entity/handle.hpp>
#include <entt/entity/helper.hpp>

#include <filesystem>

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

	//TODO: align to cache line
	struct ACRefsCounters final : AssetComponent
	{
		// LiveHandles[ AssetLoadingPriority::None ]
		// is used to count how many assets keep this asset loaded as dependence (eg shading model loaded for multiple materials)
		// so the first can load and last unload this asset
		std::atomic<int> LiveHandles[AssetLoadingPriority::Count] = {};

		bool ActiveUser = false; //TODO: make this a shared_mutex
	};

	struct ACGPUBuffer_OpenGL final : public AssetComponent
	{
		Resource::RStaticBuffer_OpenGL Buffer;
	};
}