#pragma once

#include "FoolsEngine/Core/Core.h"
#include "Asset.h"

#include <yaml-cpp\yaml.h>

#include <unordered_map>

namespace fe
{
	class AssetSourceRegistry
	{
	public:
		static void Add(const AssetSource& assetSource);
		
		static bool Exist(AssetSourceID ID)                  { return s_Data.s_MapByID.find(ID)     != s_Data.s_MapByID.end();   }
		static bool Exist(const std::filesystem::path& path) { return s_Data.m_MapByPath.find(path) != s_Data.m_MapByPath.end(); }
		
		static AssetSource* Get(AssetSourceID ID)
		{
			FE_CORE_ASSERT(Exist(ID), "AssetSource not found");
			return s_Data.s_MapByID[ID];
		}
		static AssetSource* Get(const std::filesystem::path& path)
		{
			FE_CORE_ASSERT(Exist(path), "AssetSource not found");
			return s_Data.m_MapByPath[path];
		}

		static const std::vector<AssetSource>& GetAll() { return s_Data.m_AssetSources; }

		bool Serialize(YAML::Emitter& emitter)
		{
			return true;
		}
		bool Deserialize(YAML::Node& node)
		{
			return true;
		}

	private:
		struct InternalData
		{
			std::vector<AssetSource> m_AssetSources;
			std::unordered_map<AssetSourceID        , AssetSource*> s_MapByID;
			std::unordered_map<std::filesystem::path, AssetSource*> m_MapByPath;
		};
		static InternalData s_Data;
	};

	class AssetProxyRegistry
	{
	public:
		static void Add(const AssetProxy& assetProxy);

		static bool Exist(AssetProxyID ID) { return s_Data.s_MapByID.find(ID) != s_Data.s_MapByID.end(); }
		static bool Exist(const std::filesystem::path& path) { return s_Data.m_MapByPath.find(path) != s_Data.m_MapByPath.end(); }

		static AssetProxy* Get(AssetProxyID ID)
		{
			FE_CORE_ASSERT(Exist(ID), "AssetProxy not found");
			return s_Data.s_MapByID[ID];
		}
		static AssetProxy* Get(const std::filesystem::path& path)
		{
			FE_CORE_ASSERT(Exist(path), "AssetProxy not found");
			return s_Data.m_MapByPath[path];
		}

		static const std::vector<AssetProxy>& GetAll() { return s_Data.m_AssetProxies; }

		bool Serialize(YAML::Emitter& emitter)
		{
			return true;
		}
		bool Deserialize(YAML::Node& node)
		{
			return true;
		}

	private:
		struct InternalData
		{
			std::vector<AssetProxy> m_AssetProxies;

			std::unordered_map<AssetProxyID         , AssetProxy*> s_MapByID;
			std::unordered_map<std::filesystem::path, AssetProxy*> m_MapByPath;
		};
		static InternalData s_Data;
	};

	class AssetSignatureRegistry
	{
	public:
		static AssetSignature* GenerateNew()
		{
			AssetSignature assetSignature;
			s_MapByID[assetSignature.ID] = assetSignature;
		}

		static bool Exist(AssetID assetID) { return s_MapByID.find(assetID) != s_MapByID.end(); }
		
		static AssetSignature& Get(AssetID assetID)
		{
			FE_CORE_ASSERT(Exist(assetID), "Asset not found");
			return s_MapByID[assetID];
		}

		bool Serialize(YAML::Emitter& emitter)
		{
			return true;
		}
		bool Deserialize(YAML::Node& node)
		{
			return true;
		}

	private:
		static std::unordered_map<AssetID, AssetSignature> s_MapByID;
	};
}