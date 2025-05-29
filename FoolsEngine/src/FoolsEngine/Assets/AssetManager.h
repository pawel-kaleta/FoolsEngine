#pragma once

#include "Asset.h"

namespace fe
{
	template <typename tnAsset>
	class AssetHandle;

	class AssetManager
	{
	public:
		static AssetRegistry& GetRegistry() { return s_Instance->m_Registry; }

#ifdef FE_EDITOR
		template <typename tnAsset>
		static AssetID CreateEditorAsset()
		{
			FE_PROFILER_FUNC();

			AssetRegistry& reg = s_Instance->m_Registry;
			AssetID assetID = reg.create();

			reg.emplace<ACAssetType>(assetID).Type = tnAsset::GetTypeStatic();
			reg.emplace<tnAsset::Core>(assetID).Init();

			return assetID;
		}
#endif
		template <typename tnAsset>
		static AssetID CreateBaseAsset(UUID uuid)
		{
			FE_PROFILER_FUNC();

			AssetRegistry& reg = s_Instance->m_Registry;
			AssetID assetID = reg.create();

			reg.emplace<ACUUID>(assetID).UUID = uuid;
			s_Instance->m_MapByUUID[uuid] = assetID;

			reg.emplace<ACAssetType>(assetID).Type = tnAsset::GetTypeStatic();
			reg.emplace<tnAsset::Core>(assetID).Init();

			return assetID;
		}
		template <typename tnAsset>
		static AssetID CreateInternalAsset(AssetID parent)
		{
			FE_PROFILER_FUNC();

			AssetRegistry& reg = s_Instance->m_Registry;
			AssetID assetID = reg.create();

			auto uuid = reg.emplace<ACUUID>(assetID).UUID;
			s_Instance->m_MapByUUID[uuid] = assetID;

			reg.emplace<ACAssetType>(assetID).Type = tnAsset::GetTypeStatic();
			reg.emplace<ACMasterAsset>(assetID).Parent = parent;
			reg.emplace<tnAsset::Core>(assetID).Init();

			return assetID;
		}
		template <typename tnAsset>
		static AssetID CreateProjectAsset(const std::filesystem::path& path)
		{
			FE_PROFILER_FUNC();

			AssetRegistry& reg = s_Instance->m_Registry;
			AssetID assetID = reg.create();

			reg.emplace<ACAssetType>(assetID).Type = tnAsset::GetTypeStatic();
			reg.emplace<ACRefsCounters>(assetID);
			reg.emplace<ACFilepath>(assetID).Filepath = path;
			auto uuid = reg.emplace<ACUUID>(assetID).UUID;

			s_Instance->m_MapByUUID[uuid] = assetID;
			s_Instance->m_MapByFilepath[path] = assetID;

			reg.emplace<tnAsset::Core>(assetID).Init();

			return assetID;
		}

		template <typename tnAsset>
		static AssetID GetOrCreateAssetWithUUID(UUID uuid)
		{
			FE_CORE_ASSERT(false, "Not implemented");

			auto& Inst = *s_Instance;
			auto result = Inst.m_MapByUUID.find(uuid);
			if (result == Inst.m_MapByUUID.end())
			{

				return NullEntityID;
			}

			FE_CORE_ASSERT(Inst.m_Registry.get<ACAssetType>(result->second).Type == tnAsset::GetTypeStatic(), "Wrong asset type");

			return result->second;
		}

		void SetSourcePath(AssetID assetID, std::filesystem::path sourcePath)
		{
			AssetRegistry& reg = m_Registry;
			auto ac_path = reg.try_get<ACSourceFilepath>(assetID);
			if (ac_path)
			{
				auto& assets = m_SourceFileRegistry[ac_path->Filepath];
				auto it = assets.begin();
				for (; it != assets.end(); it++)
				{
					if (*it == assetID)
					{
						assets.erase(it);
						break;
					}
				}
			}
			else
			{
				ac_path = &(reg.emplace<ACSourceFilepath>(assetID));
			}

			ac_path->Filepath = sourcePath;
			m_SourceFileRegistry[sourcePath].push_back(assetID);
		}

		static auto GetAll() { return GetRegistry().view<AssetID>(); }
		
		static AssetID GetAssetFromFilepath(const std::filesystem::path& filepath);
		static AssetID GetAssetWithUUID(UUID uuid);

		static void EvaluateAndReload();
	private:
		friend class Application;
		AssetManager() { s_Instance = this; }
		void Shutdown() { };

		static AssetManager* s_Instance;

		AssetRegistry m_Registry;
		std::unordered_map<std::filesystem::path, AssetID> m_MapByFilepath;
		std::unordered_map<UUID, AssetID> m_MapByUUID;

		std::unordered_map<std::filesystem::path, std::vector<AssetID>> m_SourceFileRegistry;


		static void AddByFilepathMapEntry(const std::filesystem::path& path, AssetID ID)
		{
			s_Instance->m_MapByFilepath[path] = ID;
		}
		static void RemoveByFilepathMapEntry(const std::filesystem::path& path, AssetType type)
		{
			s_Instance->m_MapByFilepath.erase(path);
		}
	};
}