#pragma once

#include "FoolsEngine/Assets/AssetHandle.h"
#include "FoolsEngine/Foundation/Containers/StringBuilder.h"
#include "FoolsEngine/Foundation/Context/Pile.h"


#include <yaml-cpp/yaml.h>
#include <glm/glm.hpp>

namespace fe
{
	class Entity;

	YAML::Emitter& operator<<(YAML::Emitter& out, const Entity& entity);

	template <UInt count, typename tType>
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec<count, tType>& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq;
		for (UInt i = 0; i < count; i++)
		{
			out << v[i];
		}
		out << YAML::EndSeq;
		return out;
	}
	
	template <typename tnAsset>
	YAML::Emitter& operator<<(YAML::Emitter& out, const AssetHandle<tnAsset>& assetHandle)
	{
		out << YAML::BeginMap;
		if (assetHandle.IsValid())
		{
			out << YAML::Key << "UUID" << YAML::Value << assetHandle.GetUUID();
			out << YAML::Key << "Name" << YAML::Value << assetHandle.Observe().GetFilepath().filename().string().c_str();
			out << YAML::Key << "Loading Priority" << YAML::Value << assetHandle.GetLoadingPriority().ToCString().Data;
		}
		else
		{
			out << YAML::Key << "UUID" << YAML::Value << UUID(0);
			out << YAML::Key << "Name" << YAML::Value << "empty handle";
			out << YAML::Key << "Loading Priority" << YAML::Value << assetHandle.GetLoadingPriority().ToCString().Data;
		}
		out << YAML::EndMap;
		return out;
	}
}

namespace YAML
{
	template<fe::UInt count, typename tType>
	struct convert<glm::vec<count, tType>>
	{
		static Node encode(const glm::vec<count, tType>& rhs)
		{
			Node node;
			for (fe::UInt i=0; i<count; i++)
				node.push_back(rhs[i]);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec<count, tType>& rhs)
		{
			if (!node.IsSequence() || node.size() != count)
				return false;

			for (fe::UInt i = 0; i < count; i++)
			{
				rhs[i] = node[i].as<tType>();
			}

			return true;
		}
	};

	template<>
	struct convert<fe::String>
	{
		static Node encode(const fe::String& rhs)
		{
			fe::Pile p;
			Node node;
			node.push_back(fe::GetCString(rhs, &p).Data);
			return node;
		}
	
		static bool decode(const Node& node, fe::String& rhs)
		{
			if (!node.IsScalar())
				return false;
			
			rhs = fe::CopyString(fe::String(node.Scalar().c_str(), node.Scalar().size()+1), fe::Context::Alloc::Output);
			
			return true;
		}
	};

	template<>
	struct convert<fe::UUID>
	{
		static Node encode(const fe::UUID& rhs)
		{
			Node node;
			node.push_back(rhs);
			return node;
		}

		static bool decode(const Node& node, fe::UUID& rhs)
		{
			if (!node.IsScalar())
				return false;
			rhs = node.as<uint64_t>();
			return true;
		}
	};

	template<typename tnAsset>
	struct convert<fe::AssetHandle<tnAsset>>
	{
		static Node encode(const fe::AssetHandle<tnAsset>& rhs)
		{
			FE_CORE_ASSERT(false, "Use emitter stream operator instead");
			return Node();
		}

		static bool decode(const Node& node, fe::AssetHandle<tnAsset>& rhs)
		{
			if (!node.IsMap())
				return false;

			fe::UUID uuid = node["UUID"].as<fe::UUID>();

			fe::AssetLoadingPriority loading_priority; loading_priority.FromString(fe::String(node["Loading Priority"].as<std::string>().c_str()));

			fe::AssetID id;
			
			if (uuid == fe::UUID(0))
			{
				id = fe::NullAssetID;
			}
			else
			{
				id = fe::AssetManager::GetOrCreateAssetWithUUID(uuid);
				FE_CORE_ASSERT(id != fe::NullAssetID, "Failed to deserialize asset handle");
			}

			rhs = fe::AssetHandle<tnAsset>(id);
			rhs.SetLoadingPriority(loading_priority);

			return true;
		}
	};
}