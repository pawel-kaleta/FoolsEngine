#pragma once

#include "Asset.h"
#include "AssetManager.h"

#include "FoolsEngine\Debug\Asserts.h"

namespace fe
{
	class AssetInterface
	{
	public:
		bool IsValid() const { return (bool)m_ECSHandle; }
		AssetID GetID() const { return m_ECSHandle.entity(); }

		UUID GetUUID() const { return Get<ACUUID>().UUID; }

		const ACSourceFilepath* GetSourceFilepath() const { return GetIfExist<ACSourceFilepath>(); }
		const std::filesystem::path& GetFilepath() const { return Get<ACFilepath>().Filepath; }

		AssetType GetType() const { return Get<ACAssetType>().Type; }

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

		auto GetRefCounters() const { return GetIfExist<ACRefsCounters>(); }
	protected:
		AssetInterface() = default;
		AssetInterface(AssetType type, AssetID assetID) :
			m_ECSHandle(ECS_AssetHandle(AssetManager::Get().m_Registry, assetID))
		{ }
		AssetInterface(ECS_AssetHandle ECS_handle) :
			m_ECSHandle(std::move(ECS_handle))
		{ }

		template<typename tnAssetFlagComponent>
		void Flag() const
		{
			static_assert(std::is_base_of_v<AssetFlagComponent, tnAssetFlagComponent>, "This is not a flag component!");
			FE_CORE_ASSERT(!AnyOf<tnAssetFlagComponent>(), "This Entity already have this flag component");
			m_ECSHandle.emplace<tnAssetFlagComponent>();
		}

		template<typename tnAssetFlagComponent>
		void UnFlag() const
		{
			m_ECSHandle.remove<tnAssetFlagComponent>();
		}

		template<typename tnAssetComponent, typename... Args>
		tnAssetComponent& Emplace(Args&&... args) const
		{
			FE_CORE_ASSERT(IsValid(), "AssetHandle is not valid!");

			static_assert(std::is_base_of_v<AssetComponent, tnAssetComponent>, "This is not a component of asset!");

			FE_CORE_ASSERT(!AnyOf<tnAssetComponent>(), "This Asset already have this component");
			return m_ECSHandle.emplace<tnAssetComponent>(std::forward<Args>(args)...);
		}

		template<typename tnAssetComponent, typename... Args>
		tnAssetComponent& Replace(Args&&... args) const
		{
			FE_CORE_ASSERT(IsValid(), "AssetHandle is not valid!");

			FE_CORE_ASSERT(AllOf<tnAssetComponent>(), "This Asset does not have this component yet");
			return m_ECSHandle.replace<tnAssetComponent>(std::forward<Args>(args)...);
		}

		template<typename tnAssetComponent, typename... Args>
		tnAssetComponent& EmplaceOrReplace(Args&&... args) const
		{
			FE_CORE_ASSERT(IsValid(), "AssetHandle is not valid!");

			static_assert(std::is_base_of_v<AssetComponent, tnAssetComponent>, "This is not a component of asset!");

			return m_ECSHandle.emplace_or_replace<tnAssetComponent>(std::forward<Args>(args)...);
		}

		template<typename... tnAssetComponents>
		auto& Get() const
		{
			FE_CORE_ASSERT(IsValid(), "AssetHandle is not valid!");

			FE_CORE_ASSERT(AllOf<tnAssetComponents...>(), "This Asset does not have all of this components");
			return m_ECSHandle.get<tnAssetComponents...>();
		}

		template<typename tnAssetComponent, typename... Args>
		tnAssetComponent& GetOrEmplace(Args&&... args) const
		{
			FE_CORE_ASSERT(IsValid(), "AssetHandle is not valid!");

			static_assert(std::is_base_of_v<AssetComponent, tnAssetComponent>, "This is not a component of asset!");

			return m_ECSHandle.get_or_emplace<tnAssetComponent>(std::forward<Args>(args)...);
		}

		template<typename... tnAssetComponents>
		auto GetIfExist() const
		{
			FE_CORE_ASSERT(IsValid(), "AssetHandle is not valid!");

			return m_ECSHandle.try_get<tnAssetComponents...>();
		}

		template<typename tnAssetComponent>
		void Erase() const { m_ECSHandle.erase<tnAssetComponent>(); }

		ECS_AssetHandle m_ECSHandle;
	};

	template <typename tnAsset>
	concept AssetConcept = requires(tnAsset asset, tnAsset::User user, tnAsset::Observer observer, tnAsset::Core core)
	{
		std::is_base_of_v<Asset, tnAsset>;
		std::is_base_of_v<AssetInterface, decltype(observer)>;
		std::is_base_of_v<decltype(observer), decltype(user)>;
		asset.GetTypeStatic();
	};

	// AssetObserver<> and AssetUser<> are not using AssetConcept as a 'constraint',
	// because their argument is sometimes forward declared (incomplete type fails constraint verification)
	// instead there should be static_assert(AssetConcept<tnAsset>) for every tnAsset class in AssetTypes.h
}