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
		static void SerializeToFile(const AssetObserver<Scene>& scene);
		static bool DeserializeFromFile(const AssetUser<Scene>& scene);

		static std::string SerializeToString(const AssetObserver<Scene>& scene);
		static bool DeserializeFromString(const AssetUser<Scene>& scene, const std::string& buffer);
	private:
		static void Serialize(const AssetObserver<Scene>& scene, YAML::Emitter& emitter);
		static bool Deserialize(const AssetUser<Scene>& scene, YAML::Node& node);

		static void SerializeGameplayWorld(const GameplayWorld* world, YAML::Emitter& emitter);
		static void SerializeSystems(const GameplayWorld* world, YAML::Emitter& emitter);
		static void SerializeActors(const GameplayWorld* world, YAML::Emitter& emitter);
		static void SerializeBehaviors(const CActorData& actorData, YAML::Emitter& emitter);
		static void SerializeActorEntities(EntityID actorID, const GameplayWorld* world, YAML::Emitter& emitter);
		static void SerializeEntity(Entity entity, YAML::Emitter& emitter);
		static void SerializeEntityNode(Entity entity, YAML::Emitter& emitter);
		static void SerializeTransform(const Transform& transform, YAML::Emitter& emitter);
		static void SerializeDataComponents(BaseEntity entity, YAML::Emitter& emitter);

		static bool DeserializeGameplayWorld(GameplayWorld* world, const YAML::Node& data);

		static bool DeserializeSystems(GameplayWorld* world, const YAML::Node& data);
		template <SimulationStage::ValueType stage>
		static bool DeserializeSystemUpdates(const YAML::Node& stageUpdates, SystemsDirector* director);

		static bool DeserializeActors(GameplayWorld* world, YAML::Node& data);

		static bool DeserializeBehaviors(Actor& actor, YAML::Node& data);
		template <SimulationStage::ValueType stage>
		static bool DeserializeBehaviorUpdates(const YAML::Node& stageUpdates, Actor& actor);

		static bool DeserializeEntities(GameplayWorld* world, YAML::Node& data);

		static bool DeserializeEntityNode(const YAML::Node& data, CEntityNode& node, GameplayWorld* world);
		static bool DeserializeTransform(const YAML::Node& data, Transform& transform);
	};
	
}