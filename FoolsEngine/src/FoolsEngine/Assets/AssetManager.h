#pragma once

#include "FoolsEngine/Core/UUID.h"

#include <entt/entity/registry.hpp>


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
		fe::UUID m_UUID;
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
		fe::UUID m_UUID;

		std::vector<AssetID> AssetIDs;
	};

	using AssetRegistry = entt::basic_registry<AssetID>;

	using ECS_AssetHandle = entt::basic_handle<AssetRegistry>;
	using Const_ECS_AssetHandle = entt::basic_handle<const AssetRegistry>;


	class AssetManager
	{
	public:
		AssetManager() = delete;

		template <typename tAssetBody>
		constexpr static AssetRegistry* GetRegistry() { return &(s_Registries[tAssetBody::GetStaticType()]); }

		template <typename tAssetBody>
		AssetID NewID() { return AssetManager::GetRegistry<tAssetBody>()->create(); }
	private:
		static AssetRegistry s_Registries[AssetType::TypesCount];
		static std::unordered_map<fe::UUID, AssetID> s_AssetMapByUUID[AssetType::TypesCount];

		static std::vector<AssetSource> s_AssetSources;
		struct AssetSourceMaps
		{
			static std::unordered_map<UUID, AssetSourceID> ByUUID;
			static std::unordered_map<std::filesystem::path, AssetSourceID> ByFilepath;
		};
		static AssetSourceMaps s_AssetSourceMaps;
	};

}