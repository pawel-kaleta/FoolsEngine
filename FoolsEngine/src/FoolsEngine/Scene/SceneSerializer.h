#pragma once

#include "Scene.h"
#include "GameplayWorld\Entity.h"

#include <yaml-cpp\yaml.h>

namespace fe
{
	class SceneSerializer
	{
	public:
		static void SerializeYAML(const Ref<Scene> scene, const std::string& filepath);
		static void SerializeBinary(const Ref<Scene> scene, const std::string& filepath);

		static void DeserializeYAML(const Ref<Scene> scene, const std::string& filepath);
		static void DeserializeBinary(const Ref<Scene> scene, const std::string& filepath);

	private:
		static void SerializeGameplayWorld(GameplayWorld* world, YAML::Emitter& emitter);
		static void SerializeEntity(Entity entity, YAML::Emitter& emitter);
		static void SerializeEntityNode(Entity entity, YAML::Emitter& emitter);
		static void SerializeTransform(Transform transform, YAML::Emitter& emitter);
		static void SerializeDataComponents(BaseEntity entity, YAML::Emitter& emitter);
	};

	YAML::Emitter& operator<<(YAML::Emitter& out, const Entity& entity);

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v);

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v);
}

