#pragma once

#include "FoolsEngine.h"
#include "EditorApp.h"

namespace fe
{

	template <typename tnAsset>
	class EditorAssetHandle : public tnAsset::User
	{
	public:
		static_assert(std::is_base_of_v<Asset, tnAsset>, "This is not an asset!");

		EditorAssetHandle() :
			tnAsset::User(ECS_AssetHandle())
		{ };
		~EditorAssetHandle() {}

		EditorAssetHandle(ECS_AssetHandle assetHandle) :
			tnAsset::User(assetHandle)
		{ };
		EditorAssetHandle(AssetID assetID) :
			tnAsset::User(ECS_AssetHandle(AssetManager::GetRegistry(), assetID))
		{ };

		EditorAssetHandle(const EditorAssetHandle& other) :
			tnAsset::User(other.m_ECSHandle)
		{ };
		EditorAssetHandle(EditorAssetHandle&& other) :
			tnAsset::User(other.m_ECSHandle)
		{
			other.m_ECSHandle = ECS_AssetHandle();
		};

		EditorAssetHandle& operator=(const EditorAssetHandle& other)
		{
			tnAsset::User::m_ECSHandle = other.m_ECSHandle;
			return *this;
		}
		EditorAssetHandle& operator=(EditorAssetHandle&& other)
		{
			tnAsset::User::m_ECSHandle = other.m_ECSHandle;
			other.m_ECSHandle = ECS_AssetHandle();

			return *this;
		}

		static AssetType GetTypeStatic() { return tnAsset::GetTypeStatic(); }

		// this is castable to AssetObserver<tnAsset>& and AssetUser<tnAsset>&
		// note the refs!
		// don't let it die as one of those (automated reference casting)
		operator const AssetUser    <tnAsset>& () { return *this; }
		operator const AssetObserver<tnAsset>& () { return *this; }
	};
}