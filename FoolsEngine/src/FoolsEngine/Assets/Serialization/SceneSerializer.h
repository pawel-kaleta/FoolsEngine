#pragma once

#include "FoolsEngine/Scene/GameplayWorld/Entity.h"

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
		static void SerializeToFile(const AssetObserver& scene);
		static bool DeserializeFromFile(const AssetUser& scene);

		static std::string SerializeToString(const AssetObserver& scene);
		static bool DeserializeFromString(const AssetUser& scene, const std::string& buffer);
	private:
		static void Serialize(const AssetObserver& scene, YAML::Emitter& emitter);
		static bool Deserialize(const AssetUser& scene, YAML::Node& node);

		static void SerializeEntity(Entity entity, YAML::Emitter& emitter);
		static void SerializeEntityNode(Entity entity, YAML::Emitter& emitter);

		template <SimulationStage::ValueType stage>
		static bool DeserializeSystemUpdates(const YAML::Node& stageUpdates, SystemsDirector* director);

		template <SimulationStage::ValueType stage>
		static bool DeserializeBehaviorUpdates(const YAML::Node& stageUpdates, Actor& actor);

		static bool DeserializeEntityNode(const YAML::Node& data, CEntityNode& node, GameplayWorld* world);
	};
	
}