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

		const ACSourceFilepath* GetDataFilepath() const { return GetIfExist<ACSourceFilepath>(); }
		const std::filesystem::path& GetFilepath() const { return Get<ACFilepath>().Filepath; }

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
		AssetInterface() = default;
		AssetInterface(AssetType type, AssetID assetID) :
			m_ECSHandle(ECS_AssetHandle(AssetManager::GetRegistry(), assetID))
		{ }
		AssetInterface(ECS_AssetHandle ECS_handle) :
			m_ECSHandle(std::move(ECS_handle))
		{ }

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

		auto GetRefCounters() { return GetIfExist<ACRefsCounters>(); }

		ECS_AssetHandle m_ECSHandle;
	};
}