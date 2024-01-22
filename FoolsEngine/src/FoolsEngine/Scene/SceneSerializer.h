#pragma once

#include "Scene.h"
#include "GameplayWorld\Entity.h"

#include <filesystem>
#include <yaml-cpp\yaml.h>

namespace fe
{
	struct CActorData;

	class SceneSerializerYAML
	{
	public:
		static void Serialize(const Ref<Scene> scene, const std::filesystem::path& filepath);
		static bool Deserialize(const Ref<Scene> scene, const std::filesystem::path& filepath);

	private:
		static void SerializeGameplayWorld(GameplayWorld* world, YAML::Emitter& emitter);
		static void SerializeSystems(GameplayWorld* world, YAML::Emitter& emitter);
		static void SerializeActors(GameplayWorld* world, YAML::Emitter& emitter);
		static void SerializeBehaviors(CActorData& actorData, YAML::Emitter& emitter);
		static void SerializeActorEntities(EntityID actorID, GameplayWorld* world, YAML::Emitter& emitter);
		static void SerializeEntity(Entity entity, YAML::Emitter& emitter);
		static void SerializeEntityNode(Entity entity, YAML::Emitter& emitter);
		static void SerializeTransform(Transform transform, YAML::Emitter& emitter);
		static void SerializeDataComponents(BaseEntity entity, YAML::Emitter& emitter);

		static bool DeserializeGameplayWorld(GameplayWorld* world, YAML::Node& data);
		static bool DeserializeSystems(GameplayWorld* world, YAML::Node& data);
		static bool DeserializeActors(GameplayWorld* world, YAML::Node& data);
		static bool DeserializeBehaviors(Actor& actor, YAML::Node& data);
		static bool DeserializeEntities(GameplayWorld* world, YAML::Node& data);

		template <SimulationStages::Stages stage>
		static bool DeserializeSystemUpdates(const YAML::Node& stageUpdates, SystemsDirector* director)
		{
			if (!stageUpdates)
				return false;

			for (auto& update : stageUpdates)
			{
				if (!update["System"] || !update["Priority"])
					return false;

				System* system = director->GetSystemFromUUID(update["System"].as<UUID>());
				if (!system)
				{
					FE_CORE_ASSERT(false, "Deserialization of system update enrollment failed");
					continue;
				}
				system->RegisterForUpdate<stage>(update["Priority"].as<uint32_t>());
			}

			return true;
		}

		template <SimulationStages::Stages stage>
		static bool DeserializeBehaviorUpdates(const YAML::Node& stageUpdates, Actor& actor)
		{
			if (!stageUpdates)
				return false;

			for (auto& update : stageUpdates)
			{
				if (!update["Behavior"] || !update["Priority"])
					return false;

				Behavior* behavior = actor.GetBehaviorFromUUID(update["Behavior"].as<UUID>());
				if (!behavior)
				{
					FE_CORE_ASSERT(false, "Deserialization of system update enrollment failed");
					continue;
				}
				behavior->RegisterForUpdate<stage>(update["Priority"].as<uint32_t>());
			}

			return true;
		}

		static bool DeserializeEntityNode(YAML::Node& data, CEntityNode& node, GameplayWorld* world);
		static bool DeserializeTransform(YAML::Node& data, Transform& transform);
	};

	YAML::Emitter& operator<<(YAML::Emitter& out, const Entity& entity);

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v);

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v);
}

namespace YAML
{
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
}