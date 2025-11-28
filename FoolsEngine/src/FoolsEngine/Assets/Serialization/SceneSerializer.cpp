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

		std::ofstream fout(Project::Get()->m_AssetsPath / scene.GetFilepath());
		fout << emitter.c_str();
	}

	bool SceneSerializerYAML::DeserializeFromFile(const AssetUser<Scene>& scene)
	{
#ifdef FE_INTERNAL_BUILD
		FE_PROFILER_SESSION_START("SceneLoading", "Logs/ProfileData_SceneLoading.json");
#endif // FE_INTERNAL_BUILD

		auto filepath = Project::Get()->m_AssetsPath;
		filepath /= scene.GetFilepath();
		YAML::Node node = YAML::LoadFile(filepath.string());
		
		if (!Deserialize(scene, node))
			return false;
		AssetManager::EvaluateAndReload();

#ifdef FE_INTERNAL_BUILD
		FE_PROFILER_SESSION_END();
#endif // FE_INTERNAL_BUILD

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
					emitter << YAML::Key << "Main Light" << YAML::Value << gameplay_world->GetEntityWithPrimaryDirectionalLight();
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
							emitter << YAML::Key << "UUID" << YAML::Value << system->m_UUID;
							emitter << YAML::Key << "Active" << YAML::Value << system->m_Active;

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
							emitter << YAML::Key << stage.ToConstCharPtr() << YAML::Value << YAML::BeginSeq;
							for (auto& update_enroll : gameplay_world->GetSystems().m_SystemUpdateEnrolls[i])
							{
								emitter << YAML::BeginMap;
								emitter << YAML::Key << "System" << YAML::Value << update_enroll.System->m_UUID;
								emitter << YAML::Key << "Priority" << YAML::Value << update_enroll.Priority;
								emitter << YAML::EndMap;
							}
							emitter << YAML::EndSeq;
						}
					}
					emitter << YAML::EndMap; //System Updates
				}

				// Actors
				{
					const auto& reg = gameplay_world->m_Registry;
					const auto& UUID_storage = reg.storage<CUUID>();
					const auto& name_storage = reg.storage<CEntityName>();
					const auto& actor_storage = reg.storage<CActorData>();
					const auto& node_storage = gameplay_world->m_Registry.storage<CEntityNode>();

					emitter << YAML::Key << "Actors" << YAML::Value << YAML::BeginSeq;

					for (const auto&& [actorID, actorData] : actor_storage->each())
					{
						emitter << YAML::BeginMap;
						emitter << YAML::Key << "Actor" << YAML::Value << name_storage->get(actorID).EntityName.c_str();
						emitter << YAML::Key << "UUID" << YAML::Value << UUID_storage->get(actorID).UUID;

						// Behaviors
						{
							emitter << YAML::Key << "Behaviors" << YAML::Value << YAML::BeginSeq;
							{
								for (auto& behavior : actorData.m_Behaviors)
								{
									emitter << YAML::BeginMap;
									emitter << YAML::Key << "Behavior" << YAML::Value << behavior->GetBehaviorName();
									emitter << YAML::Key << "UUID" << YAML::Value << behavior->m_UUID;
									emitter << YAML::Key << "Active" << YAML::Value << behavior->m_Active;
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
									emitter << YAML::Key << stage.ToConstCharPtr() << YAML::Value << YAML::BeginSeq;
									for (auto& update_enroll : actorData.m_UpdateEnrolls[i])
									{
										emitter << YAML::BeginMap;
										emitter << YAML::Key << "Behavior" << YAML::Value << update_enroll.Behavior->m_UUID;
										emitter << YAML::Key << "Priority" << YAML::Value << update_enroll.Priority;
										emitter << YAML::EndMap;
									}
									emitter << YAML::EndSeq;
								}
							}
							emitter << YAML::EndMap; // Updates
						}

						// Actor's Entities
						{
							std::stack<EntityID> entities_to_serialize;
							entities_to_serialize.push(actorID);
							
							emitter << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

							EntityID entity_to_serialize;
							while (entities_to_serialize.size())
							{
								entity_to_serialize = entities_to_serialize.top();
								entities_to_serialize.pop();

								if (actor_storage->contains(entity_to_serialize))
									continue;

								SerializeEntity(Entity(entity_to_serialize, gameplay_world), emitter);

								EntityID first_sibling = node_storage.get(entity_to_serialize).FirstChild;
								EntityID current = first_sibling;

								if (current != NullEntityID)
									do
									{
										if (! actor_storage->contains(current))
											entities_to_serialize.push(current);

										current = node_storage.get(current).NextSibling;
									} while (current != first_sibling && current != NullEntityID);
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
			auto& reg_items = ComponentTypesRegistry::Get().m_DataItems;

			for (auto& item : reg_items)
			{
				auto& get_ptr = item.Getter;
				auto* component = (entity.*get_ptr)();
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
		FE_PROFILER_FUNC();

		// Scene Properties
		{
			auto scene_properties = node["Scene Properties"];
			if (!scene_properties)
			{
				FE_CORE_ASSERT(false, "Deserialization failed");
				FE_LOG_CORE_ERROR("Deserialization failed");
				return false;
			}

			auto uuid_node = scene_properties["UUID"];

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
			FE_PROFILER_SCOPE("Gameplay World");

			auto gameplay_world_node = worlds["GameplayWorld"];
			if (!gameplay_world_node) return false;
			auto gameplay_world = scene.GetCoreComponent().GameplayWorld.get();
			CEntityNode* root_entity_node_component;

			// GameplayWorld Properties
			{
				auto properties_node = gameplay_world_node["Properties"];
				if (!properties_node) return false;

				UUID root_UUID = properties_node["RootID"].as<UUID>();
				auto root_entity = gameplay_world->CreateOrGetEntityWithUUID(root_UUID);

				root_entity.Emplace<CEntityName>("WorldRoot");
				root_entity.Emplace<CTransformLocal>();
				root_entity.Emplace<CTransformGlobal>();
				root_entity.Emplace<CTags>();
				root_entity.Emplace<CHeadEntity>().HeadEntity = NullEntityID;

				root_entity_node_component = &root_entity.Emplace<CEntityNode>();
				auto root_node = properties_node["RootNode"];
				if (!DeserializeEntityNode(root_node, *root_entity_node_component, gameplay_world))
					return false;

				auto camera_entity = gameplay_world->CreateEntityWithUUID(properties_node["Primary Camera"].as<UUID>());
				gameplay_world->m_PrimaryCameraEntityID = camera_entity.ID();

				auto main_light_entity = gameplay_world->CreateEntityWithUUID(properties_node["Main Light"].as<UUID>());
				gameplay_world->m_PrimaryDirectionalLightEntityID = main_light_entity.ID();
			}

			// Actors
			{
				FE_PROFILER_SCOPE("Actors");

				auto actors = gameplay_world_node["Actors"];
				if (!actors) return false;

				bool x1 = actors.IsMap();
				bool x2 = actors.IsScalar();
				bool x3 = actors.IsSequence();
				size_t x4 = actors.size();

				for (auto actor_node : actors)
				{
					if (!actor_node["Actor"]) return false;

					std::string actor_name = actor_node["Actor"].as<std::string>();
					FE_PROFILER_SCOPE(actor_name.c_str());

					if (!actor_node["UUID"]) return false;

					Actor new_actor = gameplay_world->CreateActorWithUUID(actor_node["UUID"].as<UUID>());

					// Actor's Entities
					{
						auto entities_node = actor_node["Entities"];
						if (!entities_node) return false;

						for (auto entity : entities_node)
						{
							if (!entity["UUID"] || !entity["Entity"] || !entity["Head"] || !entity["Tags"] || !entity["Node"] || !entity["Transform"])
								return false;

							UUID uuid = entity["UUID"].as<UUID>();
							std::string name = entity["Entity"].as<std::string>();
							UUID head_UUID = entity["Head"].as<UUID>();

							// TO DO: don't use BaseEntity for emplacing ProtectedComponents, as it's prohibited and will be made impossible in the future
							BaseEntity new_entity = gameplay_world->CreateOrGetEntityWithUUID(uuid);
							new_entity.Emplace<CEntityName>(name);

							auto head = gameplay_world->CreateOrGetEntityWithUUID(head_UUID);
							new_entity.Emplace<CHeadEntity>().HeadEntity = head.ID();

							new_entity.Emplace<CTags>().Local = entity["Tags"].as<uint64_t>();

							auto& entity_node = new_entity.Emplace<CEntityNode>();
							if (!DeserializeEntityNode(entity["Node"], entity_node, gameplay_world))
								return false;

							// Transform
							{
								new_entity.Emplace<CTransformGlobal>();
								new_entity.Flag<CDirtyFlag<CTransformGlobal>>();

								auto& transform = new_entity.Emplace<CTransformLocal>().Transform;
								auto transform_node = entity["Transform"];

								if (transform_node["Shift"   ]) transform.Shift    = transform_node["Shift"   ].as<glm::vec3>(); else return false;
								if (transform_node["Rotation"]) transform.Rotation = transform_node["Rotation"].as<glm::vec3>(); else return false;
								if (transform_node["Scale"   ]) transform.Scale    = transform_node["Scale"   ].as<glm::vec3>(); else return false;
							}

							for (auto& item : ComponentTypesRegistry::Get().m_DataItems)
							{
								auto& name_funk_ptr = item.GetName;
								auto component_name = (*name_funk_ptr)();

								auto component_data = entity[component_name];
								if (component_data)
								{
									auto& create_funk_ptr = item.Emplacer;
									(new_entity.*create_funk_ptr)();

									auto& get_funk_ptr = item.Getter;
									DataComponent* component = (new_entity.*get_funk_ptr)();
									component->DeserializeBase(component_data);
								}
							}
						}
					}

					// Behaviors
					{
						auto behaviors_node = actor_node["Behaviors"];
						if (!behaviors_node)	return false;

						for (auto behavior_node : behaviors_node)
						{
							if (!behavior_node["Behavior"] || !behavior_node["UUID"])
								return false;

							auto behavior_type_name = behavior_node["Behavior"].as<std::string>();
							auto* item = BehaviorsRegistry::GetItemFromName(behavior_type_name);
							if (!item)
							{
								FE_LOG_CORE_ERROR("Deserialization of {0} failed", behavior_type_name);
								continue;
							}
							auto& create_funk_ptr = item->Create;
							Behavior* new_behavior = (new_actor.*create_funk_ptr)();

							new_behavior->m_UUID = behavior_node["UUID"].as<UUID>();
							new_behavior->m_Active = behavior_node["Active"].as<bool>();
							new_behavior->Deserialize(behavior_node, gameplay_world);
							new_behavior->Initialize();
						}

						auto behavior_updates_node = actor_node["Updates"];
						if (!behavior_updates_node) return false;
						bool success = true;

#define _DeserializeBehaviorUpdates_CALL(x) success &= DeserializeBehaviorUpdates<SimulationStage::x>(behavior_updates_node[#x], new_actor);
						FE_FOR_EACH(_DeserializeBehaviorUpdates_CALL, FE_SIMULATION_STAGES);

						if (!success) return false;
					}
				}
				
			}

			// Systems
			{
				FE_PROFILER_SCOPE("Systems");

				auto systems_node = gameplay_world_node["Systems"];
				if (!systems_node) return false;

				auto director = gameplay_world->m_SystemsDirector.get();
				for (auto system_node : systems_node)
				{
					auto system_type_name = system_node["System"].as<std::string>();

					System* new_system = director->CreateSystemFromName(system_type_name);

					if (!new_system)
					{
						FE_LOG_CORE_ERROR("Deserialization of {0} failed", system_type_name);
						continue;
					}
					new_system->m_UUID = system_node["UUID"].as<uint64_t>();
					new_system->m_Active = system_node["Active"].as<bool>();
					new_system->Deserialize(system_node, gameplay_world);
					new_system->Initialize();
				}

				auto system_updates_node = gameplay_world_node["System Updates"];
				if (!system_updates_node) return false;
				bool success = true;

#define _DeserializeSystemUpdates_CALL(x) success &= DeserializeSystemUpdates<SimulationStage::x>(system_updates_node[#x], director);
				FE_FOR_EACH(_DeserializeSystemUpdates_CALL, FE_SIMULATION_STAGES);

				if (!success) return false;
			}

			// global transforms and global tags init
			gameplay_world->GetHierarchy().m_SafeOrder = false;
			gameplay_world->GetHierarchy().EnforceSafeOrder();
			gameplay_world->GetHierarchy().MakeGlobalTransformsCurrent();
			EntityID current = root_entity_node_component->FirstChild;
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