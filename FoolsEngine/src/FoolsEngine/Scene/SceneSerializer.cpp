#include "FE_pch.h"
#include "SceneSerializer.h"

#include "GameplayWorld\Actor\Actor.h"
#include "Component.h"
#include "ComponentTypesRegistry.h"

#include <fstream>

namespace YAML {

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
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
}

namespace fe
{
	YAML::Emitter& operator<<(YAML::Emitter& out, const Entity& entity)
	{
		if (entity)
			out << entity.Get<CUUID>().UUID;
		else
			out << UUID(0);

		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}

	void SceneSerializer::SerializeYAML(const Ref<Scene> scene, const std::string& filepath)
	{
		FE_LOG_CORE_DEBUG("SerializeYAML");
		YAML::Emitter emitter;
		emitter << YAML::BeginMap;
		emitter << YAML::Key << "Scene Properties" << YAML::Value << YAML::BeginMap;
		{
			emitter << YAML::Key << "Scene Name" << YAML::Value << scene->GetName();
			emitter << YAML::Key << "Primary Camera" << YAML::Value << scene->GetEntityWithPrimaryCamera();
		}
		emitter << YAML::EndMap; //Scene Properties
		emitter << YAML::Key << "Worlds" << YAML::Value << YAML::BeginMap;
		{
			SerializeGameplayWorld(scene->GetGameplayWorld(), emitter);
		}
		emitter << YAML::EndMap; //Worlds
		emitter << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << emitter.c_str();
	}

	void SceneSerializer::SerializeBinary(const Ref<Scene> scene, const std::string& filepath)
	{
		FE_CORE_ASSERT(false, "Not implemented yet");
	}

	void SceneSerializer::DeserializeYAML(const Ref<Scene> scene, const std::string& filepath)
	{

	}

	void SceneSerializer::DeserializeBinary(const Ref<Scene> scene, const std::string& filepath)
	{
		FE_CORE_ASSERT(false, "Not implemented yet");
	}

	void SceneSerializer::SerializeGameplayWorld(GameplayWorld* world, YAML::Emitter& emitter)
	{
		auto& reg = world->GetRegistry();
		auto& UUIDstorage = reg.storage<CUUID>();
		auto& nameStorage = reg.storage<CEntityName>();
		auto& actorStorage = reg.storage<CActorData>();
		auto& nodeStorage = reg.storage<CEntityNode>();

		emitter << YAML::Key << "GameplayWorld" << YAML::Value << YAML::BeginMap;
		emitter << YAML::Key <<  "World Properies" << YAML::Value << YAML::BeginMap;
		{
			emitter << YAML::Key << "RootID" << YAML::Value << Entity(RootID, world);
		}
		emitter << YAML::EndMap;
		emitter << YAML::Key <<  "Actors" << YAML::Value << YAML::BeginSeq;
		for (auto& [actorID, actorData] : actorStorage.each())
		{
			emitter << YAML::BeginMap;
			emitter << YAML::Key << "Actor" << YAML::Value << nameStorage.get(actorID).EntityName.c_str();
			emitter << YAML::Key << "UUID"  << YAML::Value << UUIDstorage.get(actorID).UUID;
			emitter << YAML::Key << "Behaviors" << YAML::Value << YAML::BeginSeq;
			{
				//behaviors
			}
			emitter << YAML::EndSeq;
			emitter << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
			{
				std::stack<EntityID> toSerialize;

				//head entity
				{
					SerializeEntity(Entity(actorID, world), emitter);

					EntityID firstSibling = nodeStorage.get(actorID).FirstChild;
					EntityID current = firstSibling;

					if (current != NullEntityID)
						do
						{
							toSerialize.push(current);
							current = nodeStorage.get(current).NextSibling;
						} while (current != firstSibling && current != NullEntityID);
				}

				EntityID entityToSerialize;
				while (toSerialize.size())
				{
					entityToSerialize = toSerialize.top();
					toSerialize.pop();

					if (actorStorage.contains(entityToSerialize))
						continue;

					SerializeEntity(Entity(entityToSerialize, world), emitter);

					EntityID firstSibling = nodeStorage.get(entityToSerialize).FirstChild;
					EntityID current = firstSibling;

					if (current != NullEntityID)
						do
						{
							toSerialize.push(current);
							current = nodeStorage.get(current).NextSibling;
						} while (current != firstSibling && current != NullEntityID);
				}	
			}
			emitter << YAML::EndSeq; //actor's entities
			emitter << YAML::EndMap; //actor
		}
		emitter << YAML::EndSeq; //actors
		emitter << YAML::EndMap; //GameplayWorld
	}

	void SceneSerializer::SerializeEntity(Entity entity, YAML::Emitter& emitter)
	{
		emitter << YAML::BeginMap;

		emitter << YAML::Key << "Entity" << YAML::Value << entity.Get<CEntityName>().EntityName.c_str();
		emitter << YAML::Key << "UUID"   << YAML::Value << entity.Get<CUUID>().UUID;
		emitter << YAML::Key << "Head"   << YAML::Value << Entity(entity.Get<CHeadEntity>().HeadEntity, entity.GetWorld());
		emitter << YAML::Key << "Tags"   << YAML::Value << entity.GetTagsHandle().GetLocal();

		emitter << YAML::Key << "Node"   << YAML::Value << YAML::BeginMap;
		SerializeEntityNode(entity, emitter);
		emitter << YAML::EndMap;

		emitter << YAML::Key << "Transform" << YAML::Value << YAML::BeginMap;
		SerializeTransform(entity.GetTransformHandle().Local(), emitter);
		emitter << YAML::EndMap;

		SerializeDataComponents(entity, emitter);

		emitter << YAML::EndMap;
	}

	void SceneSerializer::SerializeEntityNode(Entity entity, YAML::Emitter& emitter)
	{
		auto& node = entity.Get<CEntityNode>();
		auto* world = entity.GetWorld();
		
		emitter << YAML::Key << "Parent"          << YAML::Value << Entity(node.Parent, world);
		emitter << YAML::Key << "HierarchyLvl"    << YAML::Value << node.HierarchyLvl;
		emitter << YAML::Key << "PreviousSibling" << YAML::Value << Entity(node.PreviousSibling, world);
		emitter << YAML::Key << "NextSibling"     << YAML::Value << Entity(node.NextSibling, world);
		emitter << YAML::Key << "ChildrenCount"   << YAML::Value << node.ChildrenCount;
		emitter << YAML::Key << "FirstChild"      << YAML::Value << Entity(node.FirstChild, world);
	}

	void SceneSerializer::SerializeTransform(Transform transform, YAML::Emitter& emitter)
	{
		emitter << YAML::Key << "Position" << YAML::Value << transform.Position;
		emitter << YAML::Key << "Rotation" << YAML::Value << transform.Rotation;
		emitter << YAML::Key << "Scale"    << YAML::Value << transform.Scale;
	}

	void SceneSerializer::SerializeDataComponents(BaseEntity entity, YAML::Emitter& emitter)
	{
		auto& regItems = ComponentTypesRegistry::GetInstance().DataItems;

		for (auto& item : regItems)
		{
			auto& getPtr = item.Getter;
			auto* component = (entity.*getPtr)();
			if (component)
			{
				emitter << YAML::Key << component->GetComponentName() << YAML::BeginMap;
				component->Serialize(emitter);
				emitter << YAML::EndMap;
			}
		}
	}
}