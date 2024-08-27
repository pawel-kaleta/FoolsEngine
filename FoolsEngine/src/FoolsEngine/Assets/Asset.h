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
		SceneAsset,
		TextureAsset,
		Texture2DAsset,
		MeshAsset,
		ModelAsset,
		ShaderAsset,
		MaterialAsset,
		MaterialInstanceAsset,
		AudioAsset,

		Count,
		None
	};

	namespace BaseAssets
	{
		enum class Scenes
		{
			Count
		};

		enum class Textures
		{
			Count
		};

		enum class Textures2D
		{
			Default,
			FlatWhite,

			Count
		};

		enum class Meshes
		{
			Count
		};

		enum class Models
		{
			Count
		};

		enum class Shaders
		{
			Default2D,
			Default3D,

			Count
		};

		enum class Materials
		{
			Default3D,

			Count
		};

		enum class MaterialInstances
		{
			Default3D,

			Count
		};

		enum class Audio
		{
			Count
		};

		uint32_t Counts(AssetType type);

		bool IsBaseAsset(AssetID id, AssetType type);
	};

	struct AssetComponent {};

	struct ACUUID final : AssetComponent
	{
		UUID UUID;
	};

	struct ACSourceIndex
	{
		uint32_t Index;
	};

	struct ACProxyFilepath final : AssetComponent
	{
		std::filesystem::path Filepath;
	};

	struct ACRefsCounters final : AssetComponent
	{
		bool ActiveUser = false; //TODO: make this a mutex and add all other control block code
		std::atomic<int> ActiveObserversCount = 0;
		std::atomic<int> LiveHandles = 0;
	};

	struct ACDataLocation final : public AssetComponent
	{
		void* Data = nullptr;
	};

	class Asset
	{
	public:
		AssetID GetID() const { return m_ECSHandle.entity(); }
		virtual AssetType GetType() const = 0;
		static AssetType GetTypeStatic() { FE_CORE_ASSERT(false, "Cover this method in derived class!"); return AssetType::None; }

		static bool IsKnownSourceExtension(const std::filesystem::path& extension) { FE_CORE_ASSERT(false, "Cover this method in derived class!"); return false; }
		static std::string GetSourceExtensionAlias() { FE_CORE_ASSERT(false, "Cover this method in derived class!"); return ""; }
		static std::string GetProxyExtension() { FE_CORE_ASSERT(false, "Cover this method in derived class!"); return ""; }
		static std::string GetProxyExtensionAlias() { FE_CORE_ASSERT(false, "Cover this method in derived class!"); return ""; }

		bool IsValid() const { return (bool)m_ECSHandle; }

		std::string GetName() const { return Get<ACProxyFilepath>().Filepath.stem().string(); }

		ACDataLocation& GetDataLocation() { return Get<ACDataLocation>(); }
		const ACDataLocation& GetDataLocation() const { return Get<ACDataLocation>(); }

		ACProxyFilepath& GetProxyFilepath() { return Get<ACProxyFilepath>(); }
		const ACProxyFilepath& GetProxyFilepath() const { return Get<ACProxyFilepath>(); }

		std::filesystem::path& GetSourceFilepath();
		const std::filesystem::path& GetSourceFilepath() const;

		virtual void UnloadFromGPU() = 0;
		virtual void UnloadFromCPU() = 0;
	protected:
		template <typename tnAsset>
		friend class AssetHandle;
		Asset() = default;
		Asset(AssetType type, AssetID assetID);
		Asset(ECS_AssetHandle ECS_handle) :
			m_ECSHandle(std::move(ECS_handle))
		{ }

		template<typename... tnAssetComponents>
		bool AllOf() const
		{
			FE_CORE_ASSERT(IsValid(), "AssetHandle is not valid!");
			return m_ECSHandle.all_of<tnAssetComponents...>();
		}

		template<typename... tnAssetComponents>
		bool AnyOf() const
		{
			FE_CORE_ASSERT(IsValid(), "AssetHandle is not valid!");
			return m_ECSHandle.any_of<tnAssetComponents...>();
		}

		template<typename tnAssetComponent, typename... Args>
		tnAssetComponent& Emplace(Args&&... args)
		{
			FE_CORE_ASSERT(IsValid(), "AssetHandle is not valid!");

			static_assert(std::is_base_of_v<AssetComponent, tnAssetComponent>, "This is not a component of asset!");

			FE_CORE_ASSERT(!AnyOf<tnAssetComponent>(), "This Asset already have this component");
			return m_ECSHandle.emplace<tnAssetComponent>(std::forward<Args>(args)...);
		}

		template<typename tnAssetComponent, typename... Args>
		tnAssetComponent& Replace(Args&&... args)
		{
			FE_CORE_ASSERT(IsValid(), "AssetHandle is not valid!");

			FE_CORE_ASSERT(AllOf<tnAssetComponent>(), "This Asset does not have this component yet");
			return m_ECSHandle.replace<tnAssetComponent>(std::forward<Args>(args)...);
		}

		template<typename tnAssetComponent, typename... Args>
		tnAssetComponent& EmplaceOrReplace(Args&&... args)
		{
			FE_CORE_ASSERT(IsValid(), "AssetHandle is not valid!");

			static_assert(std::is_base_of_v<AssetComponent, tnAssetComponent>, "This is not a component of asset!");

			return m_ECSHandle.emplace_or_replace<tnAssetComponent>(std::forward<Args>(args)...);
		}

		template<typename... tnAssetComponents>
		auto& Get()
		{
			FE_CORE_ASSERT(IsValid(), "AssetHandle is not valid!");

			FE_CORE_ASSERT(AllOf<tnAssetComponents...>(), "This Asset does not have all of this components");
			return m_ECSHandle.get<tnAssetComponents...>();
		}

		template<typename tnAssetComponent, typename... Args>
		tnAssetComponent& GetOrEmplace(Args&&... args)
		{
			FE_CORE_ASSERT(IsValid(), "AssetHandle is not valid!");

			static_assert(std::is_base_of_v<AssetComponent, tnAssetComponent>, "This is not a component of asset!");

			return m_ECSHandle.get_or_emplace<tnAssetComponent>(std::forward<Args>(args)...);
		}

		template<typename... tnAssetComponents>
		auto GetIfExist()
		{
			FE_CORE_ASSERT(IsValid(), "AssetHandle is not valid!");

			return m_ECSHandle.try_get<tnAssetComponents...>();
		}

		template<typename tnAssetComponent>
		const auto& Get() const
		{
			FE_CORE_ASSERT(IsValid(), "AssetHandle is not valid!");

			FE_CORE_ASSERT(AllOf<tnAssetComponent>(), "This Asset does not have this component");
			return m_ECSHandle.get<tnAssetComponent>();
		}

		template<typename tnAssetComponent>
		const tnAssetComponent* GetIfExist() const
		{
			FE_CORE_ASSERT(IsValid(), "AssetHandle is not valid!");

			return m_ECSHandle.try_get<tnAssetComponent>();
		}

		template<typename tnAssetComponent>
		void Erase() { m_ECSHandle.erase<tnAssetComponent>(); }

		ECS_AssetHandle GetECSHandle() const { return m_ECSHandle; }

		ACRefsCounters& GetRefCounters() { return Get<ACRefsCounters>(); }

	private:
		ECS_AssetHandle m_ECSHandle;
	};
}