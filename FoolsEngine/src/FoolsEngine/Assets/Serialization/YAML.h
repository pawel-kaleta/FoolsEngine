#pragma once

#include "FoolsEngine\Assets\AssetHandle.h"

#include <yaml-cpp\yaml.h>
#include <glm\glm.hpp>

namespace fe
{
	class Entity;

	YAML::Emitter& operator<<(YAML::Emitter& out, const Entity& entity);

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec1& v);
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v);
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v);
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v);
	
	template <typename tnAsset>
	YAML::Emitter& operator<<(YAML::Emitter& out, const AssetHandle<tnAsset> assetHandle)
	{
		out << YAML::BeginMap;
		if (assetHandle.IsValid())
		{
			out << YAML::Key << "UUID" << YAML::Value << assetHandle.GetUUID();
			out << YAML::Key << "Name" << YAML::Value << assetHandle.Observe().GetFilepath().filename().string().c_str();
		}
		else
		{
			out << YAML::Key << "UUID" << YAML::Value << UUID(0);
			out << YAML::Key << "Name" << YAML::Value << "empty handle";
		}
		out << YAML::EndMap;
		return out;
	}
}

namespace YAML
{
	template<>
	struct convert<glm::vec1>
	{
		static Node encode(const glm::vec1& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec1& rhs)
		{
			if (!node.IsSequence() || node.size() != 1)
				return false;

			rhs.x = node[0].as<float>();

			return true;
		}
	};

	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();

			return true;
		}
	};

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();

			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
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

			fe::AssetID id = fe::AssetManager::GetOrCreateAssetWithUUID(uuid);

			FE_CORE_ASSERT(id != fe::NullAssetID, "Failed to deserialize asset handle");

			rhs = fe::AssetHandle<tnAsset>(id);

			return true;
		}
	};
}