#pragma once

#include "FoolsEngine.h"
#include "EditorApp.h"

namespace fe
{
	class EditorAssetHandle : public AssetInterface
	{
	public:
		EditorAssetHandle() :
			AssetInterface(ECS_AssetHandle())
		{ };
		~EditorAssetHandle() {}

		EditorAssetHandle(ECS_AssetHandle assetHandle) :
			AssetInterface(assetHandle)
		{ };
		EditorAssetHandle(AssetID assetID) :
			AssetInterface(ECS_AssetHandle(AssetManager::Get().m_Registry, assetID))
		{ };

		EditorAssetHandle(const EditorAssetHandle& other) :
			AssetInterface(other.m_ECSHandle)
		{ };
		EditorAssetHandle(EditorAssetHandle&& other) noexcept :
			AssetInterface(other.m_ECSHandle)
		{
			other.m_ECSHandle = ECS_AssetHandle();
		};

		EditorAssetHandle& operator=(const EditorAssetHandle& other)
		{
			AssetInterface::m_ECSHandle = other.m_ECSHandle;
			return *this;
		}
		EditorAssetHandle& operator=(EditorAssetHandle&& other) noexcept
		{
			AssetInterface::m_ECSHandle = other.m_ECSHandle;
			other.m_ECSHandle = ECS_AssetHandle();

			return *this;
		}

		static AssetType GetTypeStatic() { return GetTypeStatic(); }

		// this is castable to AssetObserver& and AssetUser&, as all 3 are empty wrappers around AssetInterface
		// note the refs!
		// don't let it die as one of those (automated reference casting)
		operator const AssetUser& ()     { return *reinterpret_cast<AssetUser*>(this); }
		operator const AssetObserver& () { return *reinterpret_cast<AssetObserver*>(this); }
	};
}