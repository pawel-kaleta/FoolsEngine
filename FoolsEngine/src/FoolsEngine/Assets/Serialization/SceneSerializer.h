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

		static void SerializeEntity(Entity entity, YAML::Emitter& emitter);
		static void SerializeEntityNode(Entity entity, YAML::Emitter& emitter);

		template <SimulationStage::ValueType stage>
		static bool DeserializeSystemUpdates(const YAML::Node& stageUpdates, SystemsDirector* director);

		template <SimulationStage::ValueType stage>
		static bool DeserializeBehaviorUpdates(const YAML::Node& stageUpdates, Actor& actor);

		static bool DeserializeEntityNode(const YAML::Node& data, CEntityNode& node, GameplayWorld* world);
	};
	
}