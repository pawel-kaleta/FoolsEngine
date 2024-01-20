#include "FE_pch.h"
#include "SceneSerializer.h"

#include "GameplayWorld\Actor\Actor.h"
#include "Component.h"
#include "ComponentTypesRegistry.h"
#include "GameplayWorld\Actor\BehaviorsRegistry.h"

#include <fstream>


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

	void SceneSerializerYAML::Serialize(const Ref<Scene> scene, const std::filesystem::path& filepath)
	{
		YAML::Emitter emitter;
		emitter << YAML::BeginMap;
		emitter << YAML::Key << "Scene Properties" << YAML::Value << YAML::BeginMap;
		{
			emitter << YAML::Key << "Name" << YAML::Value << scene->GetName();
			emitter << YAML::Key << "UUID" << YAML::Value << scene->GetUUID();
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

	void SceneSerializerYAML::SerializeGameplayWorld(GameplayWorld* world, YAML::Emitter& emitter)
	{
		emitter << YAML::Key << "GameplayWorld" << YAML::Value << YAML::BeginMap;

		emitter << YAML::Key << "Properties" << YAML::Value << YAML::BeginMap;
		{
			emitter << YAML::Key << "RootID" << YAML::Value << Entity(RootID, world);
			emitter << YAML::Key << "RootNode" << YAML::Value << YAML::BeginMap;
			SerializeEntityNode(Entity(RootID, world), emitter);
			emitter << YAML::EndMap;

			emitter << YAML::Key << "Primary Camera" << YAML::Value << world->GetEntityWithPrimaryCamera();
		}
		emitter << YAML::EndMap;
		
		SerializeSystems(world, emitter);
		SerializeActors(world, emitter);
		
		emitter << YAML::EndMap;
	}

	void SceneSerializerYAML::SerializeSystems(GameplayWorld* world, YAML::Emitter& emitter)
	{
		emitter << YAML::Key << "Systems" << YAML::Value << YAML::BeginSeq;
		{
			for (auto& system : world->GetSystems().m_Systems)
			{
				emitter << YAML::BeginMap;

				emitter << YAML::Key << "System" << YAML::Value << system->GetSystemName();
				emitter << YAML::Key << "UUID" << YAML::Value << system->GetUUID();

				system->Serialize(emitter);

				emitter << YAML::EndMap;
			}
		}
		emitter << YAML::EndSeq;
		emitter << YAML::Key << "System Updates" << YAML::Value << YAML::BeginMap;
		{
			for (int i = 0; i < (int)SimulationStages::Stages::StagesCount; i++)
			{
				emitter << YAML::Key << SimulationStages::Names[i] << YAML::Value << YAML::BeginSeq;
				for (auto& updateEnroll : world->GetSystems().m_SystemUpdateEnrolls[i])
				{
					emitter << YAML::BeginMap;
					emitter << YAML::Key << "System" << YAML::Value << updateEnroll.System->GetUUID();
					emitter << YAML::Key << "Priority" << YAML::Value << updateEnroll.Priority;
					emitter << YAML::EndMap;
				}
				emitter << YAML::EndSeq;
			}
		}
		emitter << YAML::EndMap;
	}

	void SceneSerializerYAML::SerializeActors(GameplayWorld* world, YAML::Emitter& emitter)
	{
		auto& reg = world->GetRegistry();
		auto& UUIDstorage = reg.storage<CUUID>();
		auto& nameStorage = reg.storage<CEntityName>();
		auto& actorStorage = reg.storage<CActorData>();

		emitter << YAML::Key << "Actors" << YAML::Value << YAML::BeginSeq;
		for (auto& [actorID, actorData] : actorStorage.each())
		{
			emitter << YAML::BeginMap;
			emitter << YAML::Key << "Actor" << YAML::Value << nameStorage.get(actorID).EntityName.c_str();
			emitter << YAML::Key << "UUID" << YAML::Value << UUIDstorage.get(actorID).UUID;
			
			SerializeBehaviors(actorData, emitter);
			SerializeActorEntities(actorID, world, emitter);
			
			emitter << YAML::EndMap;
		}
		emitter << YAML::EndSeq;
	}

	void SceneSerializerYAML::SerializeBehaviors(CActorData& actorData, YAML::Emitter& emitter)
	{
		emitter << YAML::Key << "Behaviors" << YAML::Value << YAML::BeginSeq;
		{
			for (auto& behavior : actorData.m_Behaviors)
			{
				emitter << YAML::BeginMap;
				emitter << YAML::Key << "Behavior" << YAML::Value << behavior->GetBehaviorName();
				emitter << YAML::Key << "UUID" << YAML::Value << behavior->GetUUID();
				behavior->Serialize(emitter);
				emitter << YAML::EndMap;
			}
		}
		emitter << YAML::EndSeq;

		emitter << YAML::Key << "Updates" << YAML::Value << YAML::BeginMap;
		{
			for (int i = 0; i < (int)SimulationStages::Stages::StagesCount; i++)
			{
				emitter << YAML::Key << SimulationStages::Names[i] << YAML::Value << YAML::BeginSeq;
				for (auto& updateEnroll : actorData.m_UpdateEnrolls[i])
				{
					emitter << YAML::BeginMap;
					emitter << YAML::Key << "Behavior" << YAML::Value << updateEnroll.Behavior->GetUUID();
					emitter << YAML::Key << "Priority" << YAML::Value << updateEnroll.Priority;
					emitter << YAML::EndMap;
				}
				emitter << YAML::EndSeq;
			}
		}
		emitter << YAML::EndMap;
	}

	void SceneSerializerYAML::SerializeActorEntities(EntityID actorID, GameplayWorld* world, YAML::Emitter& emitter)
	{
		emitter << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		
		std::stack<EntityID> toSerialize;
		auto& reg = world->GetRegistry();
		auto& nodeStorage = world->GetRegistry().storage<CEntityNode>();
		auto& actorStorage = reg.storage<CActorData>();

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
		
		emitter << YAML::EndSeq;
	}

	void SceneSerializerYAML::SerializeEntity(Entity entity, YAML::Emitter& emitter)
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

	void SceneSerializerYAML::SerializeEntityNode(Entity entity, YAML::Emitter& emitter)
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

	void SceneSerializerYAML::SerializeTransform(Transform transform, YAML::Emitter& emitter)
	{
		emitter << YAML::Key << "Position" << YAML::Value << transform.Position;
		emitter << YAML::Key << "Rotation" << YAML::Value << transform.Rotation;
		emitter << YAML::Key << "Scale"    << YAML::Value << transform.Scale;
	}

	void SceneSerializerYAML::SerializeDataComponents(BaseEntity entity, YAML::Emitter& emitter)
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

	bool SceneSerializerYAML::Deserialize(const Ref<Scene> scene, const std::filesystem::path& filepath)
	{
		YAML::Node data = YAML::LoadFile(filepath.string());

		auto& sceneProps = data["Scene Properties"];
		if (!sceneProps)
		{
			FE_CORE_ASSERT(false, "Deserialization failed");
			FE_LOG_CORE_ERROR("Deserialization failed");
			return false;
		}

		scene->m_Name = sceneProps["Name"].as<std::string>();
		scene->m_UUID = sceneProps["UUID"].as<uint64_t>();

		auto& worlds = data["Worlds"];
		if (!worlds) return false;
		return DeserializeGameplayWorld(scene->m_GameplayWorld.get(), worlds["GameplayWorld"]);
	}

	bool SceneSerializerYAML::DeserializeGameplayWorld(GameplayWorld* world, YAML::Node& data)
	{
		auto& props = data["Properties"];

		UUID rootUUID = props["RootID"].as<UUID>();
		world->m_Registry.get<CUUID>(RootID).UUID = rootUUID;
		world->m_PersistentToTransientIDsMap[rootUUID] = RootID;

		auto& rootNode = props["RootNode"];
		if (!DeserializeEntityNode(rootNode, world->m_Registry.get<CEntityNode>(RootID), world))
			return false;

		auto cameraEntity = world->CreateEntityWithUUID(props["Primary Camera"].as<UUID>());
		world->m_PrimaryCameraEntityID = cameraEntity.ID();

		DeserializeSystems(world, data);

		auto& actors = data["Actors"];
		if (actors)
			DeserializeActors(world, actors);
		else
			return false;
		
		// global transforms and global tags init
		world->GetHierarchy().EnforceSafeOrder();
		world->GetHierarchy().MakeGlobalTransformsCurrent();
		EntityID current = world->GetRegistry().get<CEntityNode>(RootID).FirstChild;
		while (current != NullEntityID)
		{
			TagsHandle(current, &world->GetRegistry()).UpdateTags();
			current = world->GetRegistry().get<CEntityNode>(current).FirstChild;
		}

		return true;
	}

	bool SceneSerializerYAML::DeserializeSystems(GameplayWorld* world, YAML::Node& data)
	{
		auto& systems = data["Systems"];
		if (!systems)
			return false;

		auto director = world->m_SystemsDirector.get();
		for (auto& system : systems)
		{
			auto systemType = system["System"].as<std::string>();

			System* newSystem = director->CreateSystemFromName(systemType);

			if (!newSystem)
			{
				//FE_CORE_ASSERT(false, "Deserialization failed");
				FE_LOG_CORE_ERROR("Deserialization of {0} failed", systemType);
				continue;
			}
			newSystem->m_UUID = system["UUID"].as<uint64_t>();
			newSystem->Deserialize((YAML::Node)system, world);
		}

		auto& systemUpdates = data["System Updates"];
		bool success = true;
		success &= DeserializeSystemUpdates<SimulationStages::FrameStart >(systemUpdates["FrameStart" ], director);
		success &= DeserializeSystemUpdates<SimulationStages::PrePhysics >(systemUpdates["PrePhysics" ], director);
		success &= DeserializeSystemUpdates<SimulationStages::Physics    >(systemUpdates["Physics"    ], director);
		success &= DeserializeSystemUpdates<SimulationStages::PostPhysics>(systemUpdates["PostPhysics"], director);
		success &= DeserializeSystemUpdates<SimulationStages::FrameEnd	 >(systemUpdates["FrameEnd"   ], director);
		return success;
	}

	bool SceneSerializerYAML::DeserializeActors(GameplayWorld* world, YAML::Node& data)
	{
		for (auto& actor : data)
		{
			if (!actor["UUID"]) return false;

			BaseEntity newActorHead = world->CreateOrGetEntityWithUUID(actor["UUID"].as<UUID>());
			auto& actorData = world->m_Registry.emplace<CActorData>(newActorHead.ID());
			Actor newActor(actorData, world);

			if (!DeserializeBehaviors(newActor, actor))
				return false;

			auto& entities = actor["Entities"];
			if (entities)
				DeserializeEntities(world, entities);
			else
				return false;
		}

		return true;
	}

	bool SceneSerializerYAML::DeserializeBehaviors(Actor& actor, YAML::Node& data)
	{
		if (!actor || !data)
			return false;

		auto& behaviors = data["Behaviors"];
		if (!behaviors)	return false;

		for (auto& behavior : behaviors)
		{
			if (!behavior["Behavior"] || !behavior["UUID"])
				return false;

			auto behaviorType = behavior["Behavior"].as<std::string>();
			auto* item = BehaviorsRegistry::GetInstance().GetItemFromName(behaviorType);
			if (!item)
			{
				//FE_CORE_ASSERT(false, "Deserialization failed");
				FE_LOG_CORE_ERROR("Deserialization of {0} failed", behaviorType);
				continue;
			}
			auto& createFunkPtr = item->Create;
			Behavior* newBehavior = (actor.*createFunkPtr)();
			newBehavior->m_UUID = behavior["UUID"].as<UUID>();
			newBehavior->Deserialize((YAML::Node)behavior, actor.GetWorld());
		}

		auto& behaviorUpdates = data["Updates"];
		if (!behaviorUpdates) return false;
		bool success = true;
		success &= DeserializeBehaviorUpdates<SimulationStages::FrameStart >(behaviorUpdates["FrameStart" ], actor);
		success &= DeserializeBehaviorUpdates<SimulationStages::PrePhysics >(behaviorUpdates["PrePhysics" ], actor);
		success &= DeserializeBehaviorUpdates<SimulationStages::Physics    >(behaviorUpdates["Physics"    ], actor);
		success &= DeserializeBehaviorUpdates<SimulationStages::PostPhysics>(behaviorUpdates["PostPhysics"], actor);
		success &= DeserializeBehaviorUpdates<SimulationStages::FrameEnd   >(behaviorUpdates["FrameEnd"   ], actor);
		return success;
	}

	bool SceneSerializerYAML::DeserializeEntities(GameplayWorld* world, YAML::Node& data)
	{
		if (!data)
			return false;

		for (auto& entity : data)
		{
			if (!entity["UUID"] || !entity["Entity"] || !entity["Head"] || !entity["Tags"] || !entity["Node"] || !entity["Transform"])
				return false;

			// TO DO: don't use BaseEntity for emplacing ProtectedComponents, as it's prohibited and will be made impossible in future
			// would be best propably for the world to emplace all protected components and deserialization only overriding data - in case of fail we have a default fallback
			BaseEntity newEntity = world->CreateOrGetEntityWithUUID(entity["UUID"].as<UUID>());
			newEntity.Emplace<CEntityName>(entity["Entity"].as<std::string>());

			auto head = world->CreateOrGetEntityWithUUID(entity["Head"].as<UUID>());
			newEntity.Emplace<CHeadEntity>().HeadEntity = head.ID();

			newEntity.Emplace<CTags>().Local = entity["Tags"].as<uint64_t>();

			auto& node = newEntity.Emplace<CEntityNode>();
			if (!DeserializeEntityNode((YAML::Node)entity["Node"], node, world))
				return false;

			newEntity.Emplace<CTransformGlobal>();
			newEntity.Flag<CDirtyFlag<CTransformGlobal>>();
			auto& transform = newEntity.Emplace<CTransformLocal>();
			if (!DeserializeTransform((YAML::Node)entity["Transform"], transform.Transform))
				return false;

			auto& compReg = ComponentTypesRegistry::GetInstance();
			for (auto& item : compReg.DataItems)
			{
				auto& nameFunkPtr = item.Name;
				auto compName = (compReg.*nameFunkPtr)();

				auto compData = entity[compName];
				if (compData)
				{
					auto& createFunkPtr = item.Emplacer;
					(newEntity.*createFunkPtr)();
					auto& getFunkPtr = item.Getter;
					DataComponent* component = (newEntity.*getFunkPtr)();
					component->Deserialize(compData);
				}
			}
		}
		return true;
	}

	bool SceneSerializerYAML::DeserializeEntityNode(YAML::Node& data, CEntityNode& node, GameplayWorld* world)
	{
		bool success = true;
		if (data["Parent"         ]) node.Parent          = world->CreateOrGetEntityWithUUID(data["Parent"         ].as<UUID>()); else success = false;
		if (data["PreviousSibling"]) node.PreviousSibling = world->CreateOrGetEntityWithUUID(data["PreviousSibling"].as<UUID>()); else success = false;
		if (data["NextSibling"    ]) node.NextSibling     = world->CreateOrGetEntityWithUUID(data["NextSibling"    ].as<UUID>()); else success = false;
		if (data["FirstChild"     ]) node.FirstChild      = world->CreateOrGetEntityWithUUID(data["FirstChild"     ].as<UUID>()); else success = false;
		if (data["HierarchyLvl"   ]) node.HierarchyLvl    = data["HierarchyLvl" ].as<uint32_t>(); else success = false;
		if (data["ChildrenCount"  ]) node.ChildrenCount   = data["ChildrenCount"].as<uint32_t>(); else success = false;
		return success;
	}

	bool SceneSerializerYAML::DeserializeTransform(YAML::Node& data, Transform& transform)
	{
		bool success = true;
		if(data["Position"]) transform.Position = data["Position"].as<glm::vec3>();	else success = false;
		if(data["Rotation"]) transform.Rotation = data["Rotation"].as<glm::vec3>();	else success = false;
		if(data["Scale"   ]) transform.Scale    = data["Scale"   ].as<glm::vec3>();	else success = false;
		return success;
	}
}