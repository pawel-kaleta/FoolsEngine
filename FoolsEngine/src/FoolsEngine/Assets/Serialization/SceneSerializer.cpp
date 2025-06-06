#include "FE_pch.h"
#include "SceneSerializer.h"

#include "YAML.h"

#include "FoolsEngine\Scene\GameplayWorld\Actor\Actor.h"
#include "FoolsEngine\Scene\Component.h"
#include "FoolsEngine\Scene\ComponentTypesRegistry.h"
#include "FoolsEngine\Scene\GameplayWorld\Actor\BehaviorsRegistry.h"
#include "AssetsRegistrySerialization.h"

#include <fstream>
#include <stack>

namespace fe
{
	void SceneSerializerYAML::SerializeToFile(const AssetObserver<Scene>& scene)
	{
		YAML::Emitter emitter;

		emitter << YAML::BeginMap;
		
		Serialize(scene, emitter);
		emitter << YAML::EndMap;

		std::ofstream fout(scene.GetFilepath());
		fout << emitter.c_str();
	}

	bool SceneSerializerYAML::DeserializeFromFile(const AssetUser<Scene>& scene)
	{
		YAML::Node node = YAML::LoadFile(scene.GetFilepath().string());
		
		if (!Deserialize(scene, node))
			return false;
		AssetManager::EvaluateAndReload();
		return true;
	}

	std::string SceneSerializerYAML::SerializeToString(const AssetObserver<Scene>& scene)
	{
		YAML::Emitter emitter;

		emitter << YAML::BeginMap;
		Serialize(scene, emitter);
		emitter << YAML::EndMap;

		std::string out = emitter.c_str();
		return out;
	}

	bool SceneSerializerYAML::DeserializeFromString(const AssetUser<Scene>& scene, const std::string& buffer)
	{
		YAML::Node node = YAML::Load(buffer);
		return Deserialize(scene, node);
	}
	
	void SceneSerializerYAML::Serialize(const AssetObserver<Scene>& scene, YAML::Emitter& emitter)
	{
		emitter << YAML::Key << "Scene Properties" << YAML::Value << YAML::BeginMap;
		{
			auto name = scene.GetFilepath().filename();

			emitter << YAML::Key << "Name" << YAML::Value << name.c_str();
			emitter << YAML::Key << "UUID" << YAML::Value << scene.GetUUID();
		}
		emitter << YAML::EndMap; //Scene Properties
		emitter << YAML::Key << "Worlds" << YAML::Value << YAML::BeginMap;
		{
			SerializeGameplayWorld(scene.GetCoreComponent().GameplayWorld.get(), emitter);
		}
		emitter << YAML::EndMap; //Worlds
	}

	bool SceneSerializerYAML::Deserialize(const AssetUser<Scene>& scene, YAML::Node& node)
	{
		auto sceneProps = node["Scene Properties"];
		if (!sceneProps)
		{
			FE_CORE_ASSERT(false, "Deserialization failed");
			FE_LOG_CORE_ERROR("Deserialization failed");
			return false;
		}

		//std::string name = sceneProps["Name"].as<std::string>();
		//if (!name.empty()) scene.SetName(name);
		//scene.SetUUID((UUID)sceneProps["UUID"].as<uint64_t>());

		auto worlds = node["Worlds"];
		if (!worlds) return false;
		if (!DeserializeGameplayWorld(scene.GetCoreComponent().GameplayWorld.get(), worlds["GameplayWorld"]))
			return false;

		return true;
	}

	void SceneSerializerYAML::SerializeGameplayWorld(const GameplayWorld* world, YAML::Emitter& emitter)
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

	void SceneSerializerYAML::SerializeSystems(const GameplayWorld* world, YAML::Emitter& emitter)
	{
		emitter << YAML::Key << "Systems" << YAML::Value << YAML::BeginSeq;
		{
			for (const auto& system : world->GetSystems().m_Systems)
			{
				emitter << YAML::BeginMap;

				emitter << YAML::Key << "System" << YAML::Value << system->GetName();
				emitter << YAML::Key << "UUID"   << YAML::Value << system->GetUUID();
				emitter << YAML::Key << "Active" << YAML::Value << system->IsActive();

				system->Serialize(emitter);

				emitter << YAML::EndMap;
			}
		}
		emitter << YAML::EndSeq;
		emitter << YAML::Key << "System Updates" << YAML::Value << YAML::BeginMap;
		{
			for (int i = 0; i < SimulationStage::Count; i++)
			{
				SimulationStage stage; stage.FromInt(i);
				emitter << YAML::Key << stage.ToString() << YAML::Value << YAML::BeginSeq;
				for (auto& updateEnroll : world->GetSystems().m_SystemUpdateEnrolls[i])
				{
					emitter << YAML::BeginMap;
					emitter << YAML::Key << "System"   << YAML::Value << updateEnroll.System->GetUUID();
					emitter << YAML::Key << "Priority" << YAML::Value << updateEnroll.Priority;
					emitter << YAML::EndMap;
				}
				emitter << YAML::EndSeq;
			}
		}
		emitter << YAML::EndMap;
	}

	void SceneSerializerYAML::SerializeActors(const GameplayWorld* world, YAML::Emitter& emitter)
	{
		const auto& reg = world->GetRegistry();
		const auto& UUIDstorage = reg.storage<CUUID>();
		const auto& nameStorage = reg.storage<CEntityName>();
		const auto& actorStorage = reg.storage<CActorData>();

		emitter << YAML::Key << "Actors" << YAML::Value << YAML::BeginSeq;
		
		for (const auto&& [actorID, actorData] : actorStorage->each())
		{
			emitter << YAML::BeginMap;
			emitter << YAML::Key << "Actor" << YAML::Value << nameStorage->get(actorID).EntityName.c_str();
			emitter << YAML::Key << "UUID" << YAML::Value << UUIDstorage->get(actorID).UUID;
			
			SerializeBehaviors(actorData, emitter);
			SerializeActorEntities(actorID, world, emitter);
			
			emitter << YAML::EndMap;
		}
		emitter << YAML::EndSeq;
	}

	void SceneSerializerYAML::SerializeBehaviors(const CActorData& actorData, YAML::Emitter& emitter)
	{
		emitter << YAML::Key << "Behaviors" << YAML::Value << YAML::BeginSeq;
		{
			for (auto& behavior : actorData.m_Behaviors)
			{
				emitter << YAML::BeginMap;
				emitter << YAML::Key << "Behavior" << YAML::Value << behavior->GetBehaviorName();
				emitter << YAML::Key << "UUID" << YAML::Value << behavior->GetUUID();
				emitter << YAML::Key << "Active" << YAML::Value << behavior->IsActive();
				behavior->Serialize(emitter);
				emitter << YAML::EndMap;
			}
		}
		emitter << YAML::EndSeq;

		emitter << YAML::Key << "Updates" << YAML::Value << YAML::BeginMap;
		{
			for (int i = 0; i < SimulationStage::Count; i++)
			{
				SimulationStage stage; stage.FromInt(i);
				emitter << YAML::Key << stage.ToString() << YAML::Value << YAML::BeginSeq;
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

	void SceneSerializerYAML::SerializeActorEntities(EntityID actorID, const GameplayWorld* world, YAML::Emitter& emitter)
	{
		emitter << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		
		std::stack<EntityID> toSerialize;
		auto& reg = world->GetRegistry();
		const auto& nodeStorage = world->GetRegistry().storage<CEntityNode>();
		const auto& actorStorage = reg.storage<CActorData>();

		//head entity
		{
			SerializeEntity(Entity(actorID, world), emitter);

			EntityID firstSibling = nodeStorage->get(actorID).FirstChild;
			EntityID current = firstSibling;

			if (current != NullEntityID)
				do
				{
					toSerialize.push(current);
					current = nodeStorage->get(current).NextSibling;
				} while (current != firstSibling && current != NullEntityID);
		}

		EntityID entityToSerialize;
		while (toSerialize.size())
		{
			entityToSerialize = toSerialize.top();
			toSerialize.pop();

			if (actorStorage->contains(entityToSerialize))
				continue;

			SerializeEntity(Entity(entityToSerialize, world), emitter);

			EntityID firstSibling = nodeStorage->get(entityToSerialize).FirstChild;
			EntityID current = firstSibling;

			if (current != NullEntityID)
				do
				{
					toSerialize.push(current);
					current = nodeStorage->get(current).NextSibling;
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

	void SceneSerializerYAML::SerializeTransform(const Transform& transform, YAML::Emitter& emitter)
	{
		emitter << YAML::Key << "Shift"    << YAML::Value << transform.Shift;
		emitter << YAML::Key << "Rotation" << YAML::Value << transform.Rotation;
		emitter << YAML::Key << "Scale"    << YAML::Value << transform.Scale;
	}

	void SceneSerializerYAML::SerializeDataComponents(BaseEntity entity, YAML::Emitter& emitter)
	{
		auto& regItems = ComponentTypesRegistry::GetDataCompItems();

		for (auto& item : regItems)
		{
			auto& getPtr = item.Getter;
			auto* component = (entity.*getPtr)();
			if (component)
			{
				emitter << YAML::Key << component->GetName() << YAML::BeginMap;
				component->SerializeBase(emitter);
				emitter << YAML::EndMap;
			}
		}
	}

	bool SceneSerializerYAML::DeserializeGameplayWorld(GameplayWorld* world, const YAML::Node& data)
	{
		auto props = data["Properties"];

		UUID rootUUID = props["RootID"].as<UUID>();
		auto rootEntity = world->CreateOrGetEntityWithUUID(rootUUID);

		rootEntity.Emplace<CEntityName>("WorldRoot");
		rootEntity.Emplace<CTransformLocal>();
		rootEntity.Emplace<CTransformGlobal>();
		rootEntity.Emplace<CTags>();
		rootEntity.Emplace<CHeadEntity>().HeadEntity = NullEntityID;

		auto& rootNode = rootEntity.Emplace<CEntityNode>();
		auto rootData = props["RootNode"];
		if (!DeserializeEntityNode(rootData, rootNode, world))
			return false;

		auto cameraEntity = world->CreateEntityWithUUID(props["Primary Camera"].as<UUID>());
		world->m_PrimaryCameraEntityID = cameraEntity.ID();

		auto actors = data["Actors"];
		if (!actors)
			return false;
		
		if (!DeserializeActors(world, actors))
			return false;
			

		if (!DeserializeSystems(world, data))
			return false;
		
		// global transforms and global tags init
		world->GetHierarchy().m_SafeOrder = false;
		world->GetHierarchy().EnforceSafeOrder();
		world->GetHierarchy().MakeGlobalTransformsCurrent();
		EntityID current = rootNode.FirstChild;
		//while (current != NullEntityID)
		//{
		//	TagsHandle(current, &world->GetRegistry()).UpdateTags();
		//	current = world->GetRegistry().get<CEntityNode>(current).FirstChild;
		//}

		return true;
	}

	bool SceneSerializerYAML::DeserializeSystems(GameplayWorld* world, const YAML::Node& data)
	{
		auto systems = data["Systems"];
		if (!systems)
			return false;

		auto director = world->m_SystemsDirector.get();
		for (auto system : systems)
		{
			auto systemType = system["System"].as<std::string>();

			System* newSystem = director->CreateSystemFromName(systemType);

			if (!newSystem)
			{
				FE_LOG_CORE_ERROR("Deserialization of {0} failed", systemType);
				continue;
			}
			newSystem->m_UUID = system["UUID"].as<uint64_t>();
			newSystem->m_Active = system["Active"].as<bool>();
			newSystem->Deserialize(system, world);
			newSystem->Initialize();
		}

		auto systemUpdates = data["System Updates"];
		bool success = true;
		success &= DeserializeSystemUpdates<SimulationStage::FrameStart >(systemUpdates["FrameStart" ], director);
		success &= DeserializeSystemUpdates<SimulationStage::PrePhysics >(systemUpdates["PrePhysics" ], director);
		success &= DeserializeSystemUpdates<SimulationStage::Physics    >(systemUpdates["Physics"    ], director);
		success &= DeserializeSystemUpdates<SimulationStage::PostPhysics>(systemUpdates["PostPhysics"], director);
		success &= DeserializeSystemUpdates<SimulationStage::FrameEnd	 >(systemUpdates["FrameEnd"   ], director);
		return success;
	}

	template<SimulationStage::ValueType stage>
	bool SceneSerializerYAML::DeserializeSystemUpdates(const YAML::Node& stageUpdates, SystemsDirector* director)
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

#define _SceneSerializerYAML_DeserializeSystemUpdates_DEF(x) template bool SceneSerializerYAML::DeserializeSystemUpdates<SimulationStage::x>(const YAML::Node&, SystemsDirector*);
	FE_FOR_EACH(_SceneSerializerYAML_DeserializeSystemUpdates_DEF, FE_SIMULATION_STAGES);

	bool SceneSerializerYAML::DeserializeActors(GameplayWorld* world, YAML::Node& data)
	{
		if (!data)
			return false;

		bool x1 = data.IsMap();
		bool x2 = data.IsScalar();
		bool x3 = data.IsSequence();
		size_t x4 = data.size();

		for (auto actor : data)
		{
			if (!actor["UUID"])
				return false;

			Actor newActor = world->CreateActorWithUUID(actor["UUID"].as<UUID>());

			auto entities = actor["Entities"];
			if (!entities)
				return false;
			
			if (!DeserializeEntities(world, entities))
				return false;
						
			if (!DeserializeBehaviors(newActor, actor))
				return false;
		}

		return true;
	}

	bool SceneSerializerYAML::DeserializeBehaviors(Actor& actor, YAML::Node& data)
	{
		if (!actor || !data)
			return false;

		auto behaviors = data["Behaviors"];
		if (!behaviors)	return false;

		for (auto behavior : behaviors)
		{
			if (!behavior["Behavior"] || !behavior["UUID"])
				return false;

			auto behaviorType = behavior["Behavior"].as<std::string>();
			auto* item = BehaviorsRegistry::GetItemFromName(behaviorType);
			if (!item)
			{
				//FE_CORE_ASSERT(false, "Deserialization failed");
				FE_LOG_CORE_ERROR("Deserialization of {0} failed", behaviorType);
				continue;
			}
			auto& createFunkPtr = item->Create;
			Behavior* newBehavior = (actor.*createFunkPtr)();
			newBehavior->m_UUID = behavior["UUID"].as<UUID>();
			newBehavior->m_Active = behavior["Active"].as<bool>();
			newBehavior->Deserialize(behavior, actor.GetWorld());
			newBehavior->Initialize();
		}

		auto behaviorUpdates = data["Updates"];
		if (!behaviorUpdates) return false;
		bool success = true;

#define _DeserializeBehaviorUpdates_CALL(x) success &= DeserializeBehaviorUpdates<SimulationStage::x>(behaviorUpdates["FrameStart" ], actor);
		FE_FOR_EACH(_DeserializeBehaviorUpdates_CALL, FE_SIMULATION_STAGES);

		return success;
	}

	template<SimulationStage::ValueType stage>
	bool SceneSerializerYAML::DeserializeBehaviorUpdates(const YAML::Node& stageUpdates, Actor& actor)
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
	
#define _SceneSerializerYAML_DeserializeBehaviorUpdates_DEF(x) template bool SceneSerializerYAML::DeserializeBehaviorUpdates<SimulationStage::x>(const YAML::Node&, Actor&);
	FE_FOR_EACH(_SceneSerializerYAML_DeserializeBehaviorUpdates_DEF, FE_SIMULATION_STAGES);

	bool SceneSerializerYAML::DeserializeEntities(GameplayWorld* world, YAML::Node& data)
	{
		if (!data)
			return false;

		for (auto entity : data)
		{
			if (!entity["UUID"] || !entity["Entity"] || !entity["Head"] || !entity["Tags"] || !entity["Node"] || !entity["Transform"])
				return false;

			UUID uuid = entity["UUID"].as<UUID>();
			std::string name = entity["Entity"].as<std::string>();
			UUID headUUID = entity["Head"].as<UUID>();

			// TO DO: don't use BaseEntity for emplacing ProtectedComponents, as it's prohibited and will be made impossible in the future
			BaseEntity newEntity = world->CreateOrGetEntityWithUUID(uuid);
			newEntity.Emplace<CEntityName>(name);

			auto head = world->CreateOrGetEntityWithUUID(headUUID);
			newEntity.Emplace<CHeadEntity>().HeadEntity = head.ID();

			newEntity.Emplace<CTags>().Local = entity["Tags"].as<uint64_t>();

			auto& node = newEntity.Emplace<CEntityNode>();
			if (!DeserializeEntityNode(entity["Node"], node, world))
				return false;

			newEntity.Emplace<CTransformGlobal>();
			newEntity.Flag<CDirtyFlag<CTransformGlobal>>();
			auto& transform = newEntity.Emplace<CTransformLocal>();
			if (!DeserializeTransform(entity["Transform"], transform.Transform))
				return false;

			for (auto& item : ComponentTypesRegistry::GetDataCompItems())
			{
				auto& nameFunkPtr = item.GetName;
				auto compName = (*nameFunkPtr)();

				auto compData = entity[compName];
				if (compData)
				{
					auto& createFunkPtr = item.Emplacer;
					(newEntity.*createFunkPtr)();
					auto& getFunkPtr = item.Getter;
					DataComponent* component = (newEntity.*getFunkPtr)();
					component->DeserializeBase(compData);
				}
			}
		}
		return true;
	}

	bool SceneSerializerYAML::DeserializeEntityNode(const YAML::Node& data, CEntityNode& node, GameplayWorld* world)
	{
		bool success = true;
		if (data["Parent"         ]) node.Parent          = world->CreateOrGetEntityWithUUID(data["Parent"         ].as<UUID>()).ID(); else success = false;
		if (data["PreviousSibling"]) node.PreviousSibling = world->CreateOrGetEntityWithUUID(data["PreviousSibling"].as<UUID>()).ID(); else success = false;
		if (data["NextSibling"    ]) node.NextSibling     = world->CreateOrGetEntityWithUUID(data["NextSibling"    ].as<UUID>()).ID(); else success = false;
		if (data["FirstChild"     ]) node.FirstChild      = world->CreateOrGetEntityWithUUID(data["FirstChild"     ].as<UUID>()).ID(); else success = false;
		if (data["HierarchyLvl"   ]) node.HierarchyLvl    = data["HierarchyLvl" ].as<uint32_t>(); else success = false;
		if (data["ChildrenCount"  ]) node.ChildrenCount   = data["ChildrenCount"].as<uint32_t>(); else success = false;
		return success;
	}

	bool SceneSerializerYAML::DeserializeTransform(const YAML::Node& data, Transform& transform)
	{
		bool success = true;
		if(data["Shift"]) transform.Shift = data["Shift"].as<glm::vec3>();	else success = false;
		if(data["Rotation"]) transform.Rotation = data["Rotation"].as<glm::vec3>();	else success = false;
		if(data["Scale"   ]) transform.Scale    = data["Scale"   ].as<glm::vec3>();	else success = false;
		return success;
	}
}