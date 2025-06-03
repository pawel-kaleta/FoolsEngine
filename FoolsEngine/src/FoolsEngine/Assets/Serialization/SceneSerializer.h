#pragma once

#include "FoolsEngine\Scene\Scene.h"
#include "FoolsEngine\Scene\GameplayWorld\Entity.h"

#include <filesystem>

namespace YAML
{
	class Emitter;
	class Node;
}

namespace fe
{
	struct CActorData;

	class SceneSerializerYAML
	{
	public:
		static void SerializeToFile(AssetUser<Scene>& scene);
		static bool DeserializeFromFile(AssetUser<Scene>& scene);

		static std::string SerializeToString(AssetUser<Scene>& scene);
		static bool DeserializeFromString(AssetUser<Scene>& scene, const std::string& buffer);
	private:
		static void Serialize(AssetUser<Scene>& scene, YAML::Emitter& emitter);
		static bool Deserialize(AssetUser<Scene>& scene, YAML::Node& node);

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
		template <SimulationStage::ValueType stage>
		static bool DeserializeSystemUpdates(const YAML::Node& stageUpdates, SystemsDirector* director);

		static bool DeserializeActors(GameplayWorld* world, YAML::Node& data);

		static bool DeserializeBehaviors(Actor& actor, YAML::Node& data);
		template <SimulationStage::ValueType stage>
		static bool DeserializeBehaviorUpdates(const YAML::Node& stageUpdates, Actor& actor);

		static bool DeserializeEntities(GameplayWorld* world, YAML::Node& data);

		static bool DeserializeEntityNode(YAML::Node& data, CEntityNode& node, GameplayWorld* world);
		static bool DeserializeTransform(YAML::Node& data, Transform& transform);
	};
	
}