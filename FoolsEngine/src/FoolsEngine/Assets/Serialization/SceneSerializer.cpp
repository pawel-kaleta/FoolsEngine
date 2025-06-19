#include "FE_pch.h"
#include "SceneSerializer.h"

#include "YAML.h"

#include "FoolsEngine\Scene\GameplayWorld\Actor\Actor.h"
#include "FoolsEngine\Scene\Component.h"
#include "FoolsEngine\Scene\ComponentTypesRegistry.h"
#include "FoolsEngine\Scene\GameplayWorld\Actor\BehaviorsRegistry.h"
#include "AssetsRegistrySerialization.h"

#include "FoolsEngine\Core\Project.h"

#include <memory_resource>
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
		auto filepath = Project::GetInstance()->AssetsPath;
		filepath /= scene.GetFilepath();
		YAML::Node node = YAML::LoadFile(filepath.string());
		
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
		//Scene Properties
		{
			emitter << YAML::Key << "Scene Properties" << YAML::Value << YAML::BeginMap;
			emitter << YAML::Key << "UUID" << YAML::Value << scene.GetUUID();
			emitter << YAML::EndMap; //Scene Properties
		}

		//Worlds
		{
			emitter << YAML::Key << "Worlds" << YAML::Value << YAML::BeginMap;

			// Gameplay World
			{
				auto gameplay_world = scene.GetCoreComponent().GameplayWorld.get();
				emitter << YAML::Key << "GameplayWorld" << YAML::Value << YAML::BeginMap;

				{
					emitter << YAML::Key << "Properties" << YAML::Value << YAML::BeginMap;
					emitter << YAML::Key << "RootID" << YAML::Value << Entity(RootID, gameplay_world);

					emitter << YAML::Key << "RootNode" << YAML::Value << YAML::BeginMap;
					SerializeEntityNode(Entity(RootID, gameplay_world), emitter);
					emitter << YAML::EndMap;

					emitter << YAML::Key << "Primary Camera" << YAML::Value << gameplay_world->GetEntityWithPrimaryCamera();
					emitter << YAML::EndMap;
				}

				// Systems
				{
					emitter << YAML::Key << "Systems" << YAML::Value << YAML::BeginSeq;
					{
						for (const auto& system : gameplay_world->GetSystems().m_Systems)
						{
							emitter << YAML::BeginMap;

							emitter << YAML::Key << "System" << YAML::Value << system->GetName();
							emitter << YAML::Key << "UUID" << YAML::Value << system->GetUUID();
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
							for (auto& updateEnroll : gameplay_world->GetSystems().m_SystemUpdateEnrolls[i])
							{
								emitter << YAML::BeginMap;
								emitter << YAML::Key << "System" << YAML::Value << updateEnroll.System->GetUUID();
								emitter << YAML::Key << "Priority" << YAML::Value << updateEnroll.Priority;
								emitter << YAML::EndMap;
							}
							emitter << YAML::EndSeq;
						}
					}
					emitter << YAML::EndMap; //System Updates
				}

				// Actors
				{
					const auto& reg = gameplay_world->GetRegistry();
					const auto& UUIDstorage = reg.storage<CUUID>();
					const auto& nameStorage = reg.storage<CEntityName>();
					const auto& actorStorage = reg.storage<CActorData>();

					emitter << YAML::Key << "Actors" << YAML::Value << YAML::BeginSeq;

					for (const auto&& [actorID, actorData] : actorStorage->each())
					{
						emitter << YAML::BeginMap;
						emitter << YAML::Key << "Actor" << YAML::Value << nameStorage->get(actorID).EntityName.c_str();
						emitter << YAML::Key << "UUID" << YAML::Value << UUIDstorage->get(actorID).UUID;

						// Behaviors
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
							emitter << YAML::EndSeq; //Behaviors

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
							emitter << YAML::EndMap; // Updates
						}

						// Actor's Entities
						{
							std::stack<EntityID> toSerialize;
							const auto& nodeStorage = gameplay_world->GetRegistry().storage<CEntityNode>();
							
							emitter << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

							//head entity
							{
								SerializeEntity(Entity(actorID, gameplay_world), emitter);

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

								if (actorStorage->contains(entityToSerialize))
									continue;

								SerializeEntity(Entity(entityToSerialize, gameplay_world), emitter);

								EntityID firstSibling = nodeStorage.get(entityToSerialize).FirstChild;
								EntityID current = firstSibling;

								if (current != NullEntityID)
									do
									{
										toSerialize.push(current);
										current = nodeStorage.get(current).NextSibling;
									} while (current != firstSibling && current != NullEntityID);
							}

							emitter << YAML::EndSeq; //Actor's Entities
						}

						emitter << YAML::EndMap; //Actor
					}
					emitter << YAML::EndSeq; //Actors
				}

				emitter << YAML::EndMap; //GameplayWorld
			}

			emitter << YAML::EndMap; //Worlds
		}
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

		// Transform
		{
			const auto& transform = entity.GetTransformHandle().Local();

			emitter << YAML::Key << "Transform" << YAML::Value << YAML::BeginMap;
			emitter << YAML::Key << "Shift"    << YAML::Value << transform.Shift;
			emitter << YAML::Key << "Rotation" << YAML::Value << transform.Rotation;
			emitter << YAML::Key << "Scale"    << YAML::Value << transform.Scale;
			emitter << YAML::EndMap;
		}

		// Data Components
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

	bool SceneSerializerYAML::Deserialize(const AssetUser<Scene>& scene, YAML::Node& node)
	{
		// Scene Properties
		{
			auto sceneProps = node["Scene Properties"];
			if (!sceneProps)
			{
				FE_CORE_ASSERT(false, "Deserialization failed");
				FE_LOG_CORE_ERROR("Deserialization failed");
				return false;
			}

			auto uuid_node = sceneProps["UUID"];

			if (uuid_node)
			{
				FE_CORE_ASSERT(uuid_node.as<UUID>() == scene.GetUUID(), "");
			}
			else
			{
				FE_CORE_ASSERT(false, "Missing uuid in scene file");
			}
		}

		auto worlds = node["Worlds"];
		if (!worlds) return false;

		// Gameplay World
		{
			auto gameplay_world_node = worlds["GameplayWorld"];
			if (!gameplay_world_node) return false;
			auto gameplay_world = scene.GetCoreComponent().GameplayWorld.get();
			CEntityNode* root_node;

			// GameplayWorld Properties
			{
				auto props = gameplay_world_node["Properties"];
				if (!props) return false;

				UUID rootUUID = props["RootID"].as<UUID>();
				auto rootEntity = gameplay_world->CreateOrGetEntityWithUUID(rootUUID);

				rootEntity.Emplace<CEntityName>("WorldRoot");
				rootEntity.Emplace<CTransformLocal>();
				rootEntity.Emplace<CTransformGlobal>();
				rootEntity.Emplace<CTags>();
				rootEntity.Emplace<CHeadEntity>().HeadEntity = NullEntityID;

				root_node = &rootEntity.Emplace<CEntityNode>();
				auto rootData = props["RootNode"];
				if (!DeserializeEntityNode(rootData, *root_node, gameplay_world))
					return false;

				auto cameraEntity = gameplay_world->CreateEntityWithUUID(props["Primary Camera"].as<UUID>());
				gameplay_world->m_PrimaryCameraEntityID = cameraEntity.ID();
			}

			// Actors
			{
				auto actors = gameplay_world_node["Actors"];
				if (!actors) return false;

				bool x1 = actors.IsMap();
				bool x2 = actors.IsScalar();
				bool x3 = actors.IsSequence();
				size_t x4 = actors.size();

				for (auto actor_node : actors)
				{
					if (!actor_node["UUID"]) return false;

					Actor newActor = gameplay_world->CreateActorWithUUID(actor_node["UUID"].as<UUID>());

					// Actor's Entities
					{
						auto entities = actor_node["Entities"];
						if (!entities) return false;

						for (auto entity : entities)
						{
							if (!entity["UUID"] || !entity["Entity"] || !entity["Head"] || !entity["Tags"] || !entity["Node"] || !entity["Transform"])
								return false;

							UUID uuid = entity["UUID"].as<UUID>();
							std::string name = entity["Entity"].as<std::string>();
							UUID headUUID = entity["Head"].as<UUID>();

							// TO DO: don't use BaseEntity for emplacing ProtectedComponents, as it's prohibited and will be made impossible in the future
							BaseEntity newEntity = gameplay_world->CreateOrGetEntityWithUUID(uuid);
							newEntity.Emplace<CEntityName>(name);

							auto head = gameplay_world->CreateOrGetEntityWithUUID(headUUID);
							newEntity.Emplace<CHeadEntity>().HeadEntity = head.ID();

							newEntity.Emplace<CTags>().Local = entity["Tags"].as<uint64_t>();

							auto& node = newEntity.Emplace<CEntityNode>();
							if (!DeserializeEntityNode(entity["Node"], node, gameplay_world))
								return false;

							// Transform
							{
								newEntity.Emplace<CTransformGlobal>();
								newEntity.Flag<CDirtyFlag<CTransformGlobal>>();

								auto& transform = newEntity.Emplace<CTransformLocal>().Transform;
								auto transform_node = entity["Transform"];

								if (transform_node["Shift"   ]) transform.Shift    = transform_node["Shift"   ].as<glm::vec3>(); else return false;
								if (transform_node["Rotation"]) transform.Rotation = transform_node["Rotation"].as<glm::vec3>(); else return false;
								if (transform_node["Scale"   ]) transform.Scale    = transform_node["Scale"   ].as<glm::vec3>(); else return false;
							}

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
					}

					// Behaviors
					{
						auto behaviors = actor_node["Behaviors"];
						if (!behaviors)	return false;

						for (auto behavior_node : behaviors)
						{
							if (!behavior_node["Behavior"] || !behavior_node["UUID"])
								return false;

							auto behaviorType = behavior_node["Behavior"].as<std::string>();
							auto* item = BehaviorsRegistry::GetItemFromName(behaviorType);
							if (!item)
							{
								//FE_CORE_ASSERT(false, "Deserialization failed");
								FE_LOG_CORE_ERROR("Deserialization of {0} failed", behaviorType);
								continue;
							}
							auto& createFunkPtr = item->Create;
							Behavior* newBehavior = (newActor.*createFunkPtr)();
							newBehavior->m_UUID = behavior_node["UUID"].as<UUID>();
							newBehavior->m_Active = behavior_node["Active"].as<bool>();
							newBehavior->Deserialize(behavior_node, newActor.GetWorld());
							newBehavior->Initialize();
						}

						auto behaviorUpdates = actor_node["Updates"];
						if (!behaviorUpdates) return false;
						bool success = true;

#define _DeserializeBehaviorUpdates_CALL(x) success &= DeserializeBehaviorUpdates<SimulationStage::x>(behaviorUpdates[#x], newActor);
						FE_FOR_EACH(_DeserializeBehaviorUpdates_CALL, FE_SIMULATION_STAGES);

						if (!success) return false;
					}
				}
				
			}

			// Systems
			{
				auto systems = gameplay_world_node["Systems"];
				if (!systems) return false;

				auto director = gameplay_world->m_SystemsDirector.get();
				for (auto system_node : systems)
				{
					auto systemType = system_node["System"].as<std::string>();

					System* newSystem = director->CreateSystemFromName(systemType);

					if (!newSystem)
					{
						FE_LOG_CORE_ERROR("Deserialization of {0} failed", systemType);
						continue;
					}
					newSystem->m_UUID = system_node["UUID"].as<uint64_t>();
					newSystem->m_Active = system_node["Active"].as<bool>();
					newSystem->Deserialize(system_node, gameplay_world);
					newSystem->Initialize();
				}

				auto systemUpdates = gameplay_world_node["System Updates"];
				if (!systemUpdates) return false;
				bool success = true;

#define _DeserializeSystemUpdates_CALL(x) success &= DeserializeSystemUpdates<SimulationStage::x>(systemUpdates[#x], director);
				FE_FOR_EACH(_DeserializeSystemUpdates_CALL, FE_SIMULATION_STAGES);

				if (!success) return false;
			}

			// global transforms and global tags init
			gameplay_world->GetHierarchy().m_SafeOrder = false;
			gameplay_world->GetHierarchy().EnforceSafeOrder();
			gameplay_world->GetHierarchy().MakeGlobalTransformsCurrent();
			EntityID current = root_node->FirstChild;
			//while (current != NullEntityID)
			//{
			//	TagsHandle(current, &gameplay_world->GetRegistry()).UpdateTags();
			//	current = gameplay_world->GetRegistry().get<CEntityNode>(current).FirstChild;
			//}
		}

		return true;
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

	bool SceneSerializerYAML::DeserializeEntityNode(const YAML::Node& data, CEntityNode& node, GameplayWorld* world)
	{
		if (data["Parent"         ]) node.Parent          = world->CreateOrGetEntityWithUUID(data["Parent"         ].as<UUID>()).ID(); else return false;
		if (data["PreviousSibling"]) node.PreviousSibling = world->CreateOrGetEntityWithUUID(data["PreviousSibling"].as<UUID>()).ID(); else return false;
		if (data["NextSibling"    ]) node.NextSibling     = world->CreateOrGetEntityWithUUID(data["NextSibling"    ].as<UUID>()).ID(); else return false;
		if (data["FirstChild"     ]) node.FirstChild      = world->CreateOrGetEntityWithUUID(data["FirstChild"     ].as<UUID>()).ID(); else return false;
		if (data["HierarchyLvl"   ]) node.HierarchyLvl    = data["HierarchyLvl" ].as<uint32_t>(); else return false;
		if (data["ChildrenCount"  ]) node.ChildrenCount   = data["ChildrenCount"].as<uint32_t>(); else return false;
		return true;
	}
}