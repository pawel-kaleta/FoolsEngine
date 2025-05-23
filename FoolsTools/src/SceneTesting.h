#pragma once

#include <FoolsEngine.h>

namespace fe
{
	struct CMovement : DataComponent
	{
		struct TargetMovement {
			float MoveSpeed = 0.5f;
			float RotationSpeed = 80.0f;

			Transform CalculateNewTransform(Transform transform)
			{
				auto& position = transform.Shift;
				auto& rotation = transform.Rotation;

				float moveDistance = Time::DeltaTime() * MoveSpeed;
				float rotAngle = Time::DeltaTime() * RotationSpeed;

				     if (InputPolling::IsKeyPressed(InputCodes::KP4))	rotation.y += rotAngle;
				else if (InputPolling::IsKeyPressed(InputCodes::KP6))	rotation.y -= rotAngle;

				     if (InputPolling::IsKeyPressed(InputCodes::Right))	position.x += moveDistance;
				else if (InputPolling::IsKeyPressed(InputCodes::Left))	position.x -= moveDistance;
				     if (InputPolling::IsKeyPressed(InputCodes::Up))	position.z -= moveDistance;
				else if (InputPolling::IsKeyPressed(InputCodes::Down))	position.z += moveDistance;

				return transform;
			}
		} Movement;

		FE_COMPONENT_SETUP(CMovement, "Movement");
		virtual void DrawInspectorWidget(BaseEntity entity) override
		{
			ImGui::DragFloat("MoveSpeed"     , &Movement.MoveSpeed     , 0.01f);
			ImGui::DragFloat("RotationSpeed" , &Movement.RotationSpeed , 0.10f);
		}

		virtual void Serialize(YAML::Emitter& emitter) override
		{
			emitter << YAML::Key << "MoveSpeed"      << YAML::Value << Movement.MoveSpeed;
			emitter << YAML::Key << "RotationSpeed"  << YAML::Value << Movement.RotationSpeed;
		}

		virtual void Deserialize(YAML::Node& data) override
		{
			Movement.MoveSpeed      = data["MoveSpeed"     ].as<float>();
			Movement.RotationSpeed  = data["RotationSpeed" ].as<float>();
		}
	};

	class PlayerMovementBehavior : public Behavior
	{
	public:
		PlayerMovementBehavior() = default;
		virtual ~PlayerMovementBehavior() override = default;

		virtual void OnUpdate_PrePhysics() override
		{
			static bool firstError = false;
			if (!(m_Player && m_Movement.GetEntity()))
			{
				if (!firstError)
				{
					FE_LOG_ERROR("Missing references on PlayerMovementBehavior");
					firstError = true;
				}
				return;
			}
			firstError = false;

			auto transform = m_Player.GetTransformHandle();
			auto newTransform = m_Movement.Get()->Movement.CalculateNewTransform(transform.Global());
			transform = newTransform;
		}

		virtual void OnActivate() override
		{
			RegisterForUpdate<SimulationStages::Stages::PrePhysics>(10);
		}

		virtual EntityID DrawInspectorWidget() override
		{
			EntityID selection = NullEntityID;

			if (DrawCompPtr(m_Movement, "Movement Component"))
				selection = m_Movement.GetEntity().ID();
			if (DrawEntity(m_Player, "Player's root"))
				selection = m_Player;
			
			return selection;
		}

		virtual void Serialize(YAML::Emitter& emitter) const override 
		{
			emitter << YAML::Key << "MovementComponent" << YAML::Value << m_Movement.GetEntity();
			emitter << YAML::Key << "Player Root entity" << YAML::Value << m_Player;
		}

		virtual void Deserialize(YAML::Node& data, GameplayWorld* world) override
		{
			m_Player = world->CreateOrGetEntityWithUUID(data["Player Root entity"].as<UUID>());
			
			auto movementEntity = world->CreateOrGetEntityWithUUID(data["MovementComponent"].as<UUID>());
			m_Movement.Set(movementEntity);
		}

		FE_BEHAVIOR_SETUP(PlayerMovementBehavior, "PlayerMovement");

		CompPtr<CMovement> m_Movement;
		Entity m_Player;
	};

	class TestBehavior : public Behavior
	{
	public:
		TestBehavior() = default;
		virtual ~TestBehavior() override = default;

		virtual void OnUpdate_PrePhysics() override { }

		virtual void OnActivate() override
		{
			RegisterForUpdate<SimulationStages::Stages::PrePhysics>(9);
		}

		FE_BEHAVIOR_SETUP(TestBehavior, "TestBehavior");
	};

	class TestBehavior2 : public Behavior
	{
	public:
		TestBehavior2() = default;
		virtual ~TestBehavior2() override = default;

		virtual void OnUpdate_PrePhysics() override { }

		virtual void OnActivate() override
		{
			RegisterForUpdate<SimulationStages::Stages::PrePhysics>(11);
		}

		FE_BEHAVIOR_SETUP(TestBehavior2, "TestBehavior2");
	};

	class TestSystem : public System
	{
	public:
		virtual void OnActivate() override
		{
			RegisterForUpdate<SimulationStages::Stages::PostPhysics>(9);
		}

		virtual void DrawInspectorWidget() override
		{
			ImGui::Text("Test System content");
		}

		FE_SYSTEM_SETUP(TestSystem, "TestSystem");

		virtual void Serialize(YAML::Emitter& emitter) const override {	}
		virtual void Deserialize(YAML::Node& data, GameplayWorld* world) override { }
	};

	class TestSystem2 : public System
	{
	public:
		virtual void OnActivate() override
		{
			RegisterForUpdate<SimulationStages::Stages::PostPhysics>(10);
		}

		virtual void DrawInspectorWidget() override
		{
			ImGui::Text("Test System 2 content");
		}

		FE_SYSTEM_SETUP(TestSystem2, "TestSystem2");
	};

	void RegisterAndLoadStuff()
	{
		static bool once = false;

		if (once)
			return;

		BehaviorsRegistry::RegisterBehavior<PlayerMovementBehavior>();
		BehaviorsRegistry::RegisterBehavior<TestBehavior>();
		BehaviorsRegistry::RegisterBehavior<TestBehavior2>();
		ComponentTypesRegistry::RegisterDataComponent<CMovement>();
		SystemsRegistry::RegisterSystem<TestSystem>();
		SystemsRegistry::RegisterSystem<TestSystem2>();

		once = true;
	}

	void TestSceneSetup(const AssetObserver<Scene>& sceneObserver)
	{
		FE_PROFILER_FUNC();
		FE_LOG_INFO("Test Scene Setup");

		auto gameplay_world = sceneObserver.GetCoreComponent().GameplayWorld.get();
		Actor enviroActor = gameplay_world->CreateActor("Enviro");

		Entity tintedTextureTile = enviroActor.CreateChildEntity("TestEntity");
		{
			auto& tile = tintedTextureTile.Emplace<CTile>().Tile;
			tile.Color = glm::vec4(0.2f, 0.7f, 0.3f, 1.0f);
			tile.TextureTilingFactor = 3;

			Transform transform;
			transform.Scale = glm::vec3(0.6f, 0.4f, 1.0f);
			transform.Rotation = glm::vec3(0.0f, 0.0f, -30.0f);
			transform.Shift = glm::vec3(0.0f, 0.2f, -0.1f);
			tintedTextureTile.GetTransformHandle() = transform;

			tintedTextureTile.Emplace<CCamera>();
			gameplay_world->SetPrimaryCameraEntity(tintedTextureTile);
		}

		Entity flatTile = enviroActor.CreateChildEntity();
		{
			auto& tile = flatTile.Emplace<CTile>().Tile;
			tile.Color = glm::vec4(0.1f, 0.1f, 1.0f, 1.0f);
			tile.TextureTilingFactor = 3;

			Transform transform;
			transform.Scale = glm::vec3(0.4f, 0.3f, 1.0f);
			transform.Rotation = glm::vec3(0.0f, 0.0f, 20.0f);
			flatTile.GetTransformHandle() = transform;
		}

		Actor childActor = enviroActor.CreateAttachedActor("TestChildActor");
		{
			auto& tile = childActor.Emplace<CTile>().Tile;
			tile.Color = glm::vec4(0.8f, 0.8f, 0.1f, 1.0f);
			tile.TextureTilingFactor = 2;

			Transform transform;
			transform.Shift = glm::vec3(0.3f, -0.2f, 0.19f);
			transform.Scale = glm::vec3(0.2f, 0.4f, 1.0f);
			transform.Rotation = glm::vec3(0.0f, 0.0f, -40.0f);
			childActor.GetTransformHandle() = transform;
		}

		Entity childOfChildActor = childActor.CreateChildEntity();

		Entity colorSprite = enviroActor.CreateChildEntity();
		{
			auto& sprite = colorSprite.Emplace<CSprite>().Sprite;
			sprite.Color = glm::vec4(0.9f, 0.2f, 0.9f, 0.8f);

			Transform transform;
			transform.Shift = glm::vec3(-0.1f, -0.1f, 0.1f);
			transform.Scale = glm::vec3(0.3f, 0.2f, 1.0f);
			colorSprite.GetTransformHandle() = transform;
		}

		Actor playerActor = gameplay_world->CreateActor("Player");
		{
			auto& sprite = playerActor.Emplace<CSprite>().Sprite;

			Transform transform;
			transform.Shift = glm::vec3(0.0f, 0.0f, 0.2f);
			transform.Scale = glm::vec3(0.3f, 0.3f, 1.0f);
			playerActor.GetTransformHandle() = transform;

			auto tags = playerActor.GetTagsHandle();
			tags.Add(Tags::Player);
			playerActor.GetTagsHandle().SetLocal(tags);

			playerActor.CreateBehavior<TestBehavior>()->Activate();
			playerActor.CreateBehavior<TestBehavior2>()->Activate();
		}

		Entity testChild_1 = playerActor.CreateChildEntity("ChildEntity_1");
		{
			auto& sprite = testChild_1.Emplace<CSprite>().Sprite;
			sprite.Color = { 1.0f, 1.0f, 1.0f, 0.5f };

			Transform transform;
			transform.Shift = glm::vec3(0.8f, 0.8f, 0.3f);
			transform.Rotation = glm::vec3(0.0f, 0.0f, 20.0f);
			transform.Scale = glm::vec3(0.5f, 0.5f, 1.0f);
			testChild_1.GetTransformHandle().SetLocal(transform);
			
			testChild_1.Emplace<CMovement>();
			PlayerMovementBehavior* movement = playerActor.CreateBehavior<PlayerMovementBehavior>();
			movement->m_Player = Entity(testChild_1);
			movement->m_Movement.Set(Entity(testChild_1));
			movement->Activate();
		}

		Entity testCild_2 = playerActor.CreateChildEntity("ChildEntity_2");
		{
			auto& sprite = testCild_2.Emplace<CSprite>().Sprite;
			sprite.Color = { 1.0f, 1.0f, 1.0f, 0.5f };

			Transform transform;
			transform.Shift = glm::vec3(-0.8f, -0.8f, 0.0f);
			transform.Rotation = glm::vec3(0.0f, 0.0f, -20.0f);
			transform.Scale = glm::vec3(0.7f, 0.4f, 1.0f);
			testCild_2.GetTransformHandle().SetLocal(transform);
		}

		gameplay_world->GetSystems().CreateSystem<TestSystem>()->Activate();
		gameplay_world->GetSystems().CreateSystem<TestSystem2>()->Activate();

		gameplay_world->GetHierarchy().MakeGlobalTransformsCurrent();
	}
}