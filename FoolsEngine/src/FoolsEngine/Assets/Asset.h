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
		PrefabAsset,
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

	struct AssetComponent {};

	struct ACUUID final : AssetComponent
	{
		UUID UUID;
	};

	struct ACName final : AssetComponent
	{
		std::string Name;
	};

	struct ACFilepath final : AssetComponent
	{
		std::filesystem::path Filepath;
	};

	struct ACDataFilepath final : AssetComponent
	{
		std::filesystem::path Filepath;
	};

	struct ACParentAsset final : AssetComponent
	{
		AssetID ParentAssetID = NullAssetID;
		AssetType ParenAssetType = AssetType::None;
	};

	struct ACRefsCounters final : AssetComponent
	{
		bool ActiveUser = false; //TODO: make this a mutex and add all other control block code
		std::atomic<int> ActiveObserversCount = 0;

		// TO DO: add AssetLoadingPriority counting
		std::atomic<int> LiveHandles = 0;

		// TO DO: split into 2 components
	};

	struct ACDataLocation final : AssetComponent
	{
		void* Data = nullptr;
	};

	class Asset
	{
	public:
		bool IsValid() const { return (bool)m_ECSHandle; }
		AssetID GetID() const { return m_ECSHandle.entity(); }

		UUID GetUUID() const { return Get<ACUUID>().UUID; }

		virtual AssetType GetType() const = 0;
		static  AssetType GetTypeStatic() { FE_CORE_ASSERT(false, "Cover this method in derived class!"); return AssetType::None; }

		      ACDataLocation& GetDataLocation()       { return Get<ACDataLocation>(); }
		const ACDataLocation& GetDataLocation() const { return Get<ACDataLocation>(); }

		      ACName* TryGetName()       { return GetIfExist<ACName>(); }
		const ACName* TryGetName() const { return GetIfExist<ACName>(); }

		void SetName(const std::string& name) { GetOrEmplace<ACName>().Name = name; }

		const ACFilepath& GetFilepath() const { return Get<ACFilepath>(); }
		void SetFilepath(const std::filesystem::path& path);

		      ACParentAsset* TryGetParentAsset()       { return GetIfExist<ACParentAsset>(); }
		const ACParentAsset* TryGetParentAsset() const { return GetIfExist<ACParentAsset>(); }

		virtual void PlaceCoreComponents() = 0;
		virtual void Release() = 0;
		
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
	protected:
		template <typename tnAsset>
		friend class AssetHandle;
		Asset() = default;
		Asset(AssetType type, AssetID assetID);
		Asset(ECS_AssetHandle ECS_handle) :
			m_ECSHandle(std::move(ECS_handle))
		{ }

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