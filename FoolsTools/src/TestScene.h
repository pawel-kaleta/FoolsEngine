#pragma once

#include <FoolsEngine.h>

namespace fe
{
	struct CPlayerMovement : DataComponent
	{
		struct TargetMovement {
			float MoveSpeed = 0.5f;
			float RotationSpeed = 80.0f;
			float ScaleStepSpeed = 0.2f;

			Transform CalculateNewTransform(Transform transform)
			{
				auto& position = transform.Position;
				auto& rotation = transform.Rotation;
				auto& scale = transform.Scale;

				float moveDistance = Time::DeltaTime() * MoveSpeed;
				float rotAngle = Time::DeltaTime() * RotationSpeed;
				float scaleStep = Time::DeltaTime() * ScaleStepSpeed;

				if (InputPolling::IsKeyPressed(InputCodes::KP1))	scale -= scaleStep;
				else if (InputPolling::IsKeyPressed(InputCodes::KP3))	scale += scaleStep;

				if (InputPolling::IsKeyPressed(InputCodes::KP4))	rotation.z -= rotAngle;
				else if (InputPolling::IsKeyPressed(InputCodes::KP6))	rotation.z += rotAngle;

				if (InputPolling::IsKeyPressed(InputCodes::Right))	position.x += moveDistance;
				else if (InputPolling::IsKeyPressed(InputCodes::Left))	position.x -= moveDistance;
				if (InputPolling::IsKeyPressed(InputCodes::Up))	position.y += moveDistance;
				else if (InputPolling::IsKeyPressed(InputCodes::Down))	position.y -= moveDistance;

				return transform;
			}
		} PlayerMovement;

		FE_COMPONENT_SETUP(CPlayerMovement, "PlayerMovement");
		virtual void DrawInspectorWidget(BaseEntity entity) override
		{
			ImGui::DragFloat("MoveSpeed", &PlayerMovement.MoveSpeed, 0.01f);
			ImGui::DragFloat("RotationSpeed", &PlayerMovement.RotationSpeed, 0.1f);
			ImGui::DragFloat("ScaleStepSpeed", &PlayerMovement.ScaleStepSpeed, 0.01f);
		}

		virtual void Serialize(YAML::Emitter& emitter) override
		{
			emitter << YAML::Key << "MoveSpeed"      << YAML::Value << PlayerMovement.MoveSpeed;
			emitter << YAML::Key << "RotationSpeed"  << YAML::Value << PlayerMovement.RotationSpeed;
			emitter << YAML::Key << "ScaleStepSpeed" << YAML::Value << PlayerMovement.ScaleStepSpeed;
		}

		virtual void Deserialize(YAML::Node& data) override
		{
			PlayerMovement.MoveSpeed      = data["MoveSpeed"     ].as<float>();
			PlayerMovement.RotationSpeed  = data["RotationSpeed" ].as<float>();
			PlayerMovement.ScaleStepSpeed = data["ScaleStepSpeed"].as<float>();
		}
	};

	class PlayerMovementBehavior : public Behavior
	{
	public:
		PlayerMovementBehavior() = default;
		virtual ~PlayerMovementBehavior() override = default;

		virtual void OnUpdate_PrePhysics() override
		{
			auto transform = m_Player.GetTransformHandle();
			auto newTransform = m_Movement.Get()->PlayerMovement.CalculateNewTransform(transform.Global());
			transform = newTransform;
		}

		virtual void OnInitialize() override
		{
			RegisterForUpdate<SimulationStages::PrePhysics>(10);
		}

		virtual void DrawInspectorWidget() override
		{
			DrawCompPtr(m_Movement, "Movement Component");
			DrawEntity(m_Player, "Player's root");
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

		CompPtr<CPlayerMovement> m_Movement;
		Entity m_Player;
	};

	class TestBehavior : public Behavior
	{
	public:
		TestBehavior() = default;
		virtual ~TestBehavior() override = default;

		virtual void OnUpdate_PrePhysics() override { }

		virtual void OnInitialize() override
		{
			RegisterForUpdate<SimulationStages::PrePhysics>(9);
		}

		FE_BEHAVIOR_SETUP(TestBehavior, "TestBehavior");
	};

	class TestBehavior2 : public Behavior
	{
	public:
		TestBehavior2() = default;
		virtual ~TestBehavior2() override = default;

		virtual void OnUpdate_PrePhysics() override { }

		virtual void OnInitialize() override
		{
			RegisterForUpdate<SimulationStages::PrePhysics>(11);
		}

		FE_BEHAVIOR_SETUP(TestBehavior2, "TestBehavior2");
	};

	class TestSystem : public System
	{
	public:
		virtual void OnInitialize() override
		{
			RegisterForUpdate<SimulationStages::PostPhysics>(9);
		}

		virtual void DrawInspectorWidget() override
		{
			ImGui::Text("Test System content");
		}

		FE_SYSTEM_SETUP(TestSystem, "TestSystem");
	};

	class TestSystem2 : public System
	{
	public:
		virtual void OnInitialize() override
		{
			RegisterForUpdate<SimulationStages::PostPhysics>(10);
		}

		virtual void DrawInspectorWidget() override
		{
			ImGui::Text("Test System 2 content");
		}

		FE_SYSTEM_SETUP(TestSystem2, "TestSystem2");
	};


	void TestSceneSetup(Scene* scene)
	{
		FE_PROFILER_FUNC();
		FE_LOG_INFO("Test Entities Spawn");

		Actor enviroActor = scene->GetGameplayWorld()->CreateActor("Enviro");

		Entity tintedTextureTile = enviroActor.CreateChildEntity("TestEntity");
		{
			auto& tile = tintedTextureTile.Emplace<CTile>().Tile;
			tile.Texture = TextureLibrary::Get("Default_Texture");
			tile.Color = glm::vec4(0.2f, 0.7f, 0.3f, 1.0f);
			tile.TextureTilingFactor = 3;

			Transform transform;
			transform.Scale = glm::vec3(0.6f, 0.4f, 1.0f);
			transform.Rotation = glm::vec3(0.0f, 0.0f, -30.0f);
			transform.Position = glm::vec3(0.0f, 0.2f, -0.1f);
			tintedTextureTile.GetTransformHandle() = transform;

			tintedTextureTile.Emplace<CCamera>();
			scene->GetGameplayWorld()->SetPrimaryCameraEntity(tintedTextureTile);
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
			transform.Position = glm::vec3(0.3f, -0.2f, 0.19f);
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
			transform.Position = glm::vec3(-0.1f, -0.1f, 0.1f);
			transform.Scale = glm::vec3(0.3f, 0.2f, 1.0f);
			colorSprite.GetTransformHandle() = transform;
		}

		Actor playerActor = scene->GetGameplayWorld()->CreateActor("Player");
		{
			TextureLibrary::Add(Texture2D::Create("assets/textures/Texture_with_Transparency.png"));

			auto& sprite = playerActor.Emplace<CSprite>().Sprite;
			sprite.Texture = TextureLibrary::Get("Texture_with_Transparency");

			Transform transform;
			transform.Position = glm::vec3(0.0f, 0.0f, 0.2f);
			transform.Scale = glm::vec3(0.3f, 0.3f, 1.0f);
			playerActor.GetTransformHandle() = transform;

			auto tags = playerActor.GetTagsHandle();
			tags.Add(Tags::Player);
			playerActor.GetTagsHandle().SetLocal(tags);

			BehaviorsRegistry::GetInstance().RegisterBehavior<PlayerMovementBehavior>();
			PlayerMovementBehavior* movement = playerActor.CreateBehavior<PlayerMovementBehavior>();
			movement->OnInitialize();
			movement->m_Player = Entity(playerActor);
			movement->m_Movement.Set(Entity(playerActor));

			BehaviorsRegistry::GetInstance().RegisterBehavior<TestBehavior>();
			BehaviorsRegistry::GetInstance().RegisterBehavior<TestBehavior2>();
			playerActor.CreateBehavior<TestBehavior>()->OnInitialize();
			playerActor.CreateBehavior<TestBehavior2>()->OnInitialize();

			playerActor.Emplace<CPlayerMovement>();
			ComponentTypesRegistry::GetInstance().RegisterDataComponent<CPlayerMovement>();
		}

		Entity testChild_1 = playerActor.CreateChildEntity("ChildEntity_1");
		{
			auto& sprite = testChild_1.Emplace<CSprite>().Sprite;
			sprite.Texture = TextureLibrary::Get("Texture_with_Transparency");
			sprite.Color = { 1.0f, 1.0f, 1.0f, 0.5f };

			Transform transform;
			transform.Position = glm::vec3(0.8f, 0.8f, 0.3f);
			transform.Rotation = glm::vec3(0.0f, 0.0f, 20.0f);
			transform.Scale = glm::vec3(0.5f, 0.5f, 1.0f);
			testChild_1.GetTransformHandle().SetLocal(transform);
		}

		Entity testCild_2 = playerActor.CreateChildEntity("ChildEntity_2");
		{
			auto& sprite = testCild_2.Emplace<CSprite>().Sprite;
			sprite.Texture = TextureLibrary::Get("Default_Texture");
			sprite.Color = { 1.0f, 1.0f, 1.0f, 0.5f };

			Transform transform;
			transform.Position = glm::vec3(-0.8f, -0.8f, 0.0f);
			transform.Rotation = glm::vec3(0.0f, 0.0f, -20.0f);
			transform.Scale = glm::vec3(0.7f, 0.4f, 1.0f);
			testCild_2.GetTransformHandle().SetLocal(transform);
		}

		SystemsRegistry::GetInstance().RegisterSystem<TestSystem>();
		scene->GetGameplayWorld()->GetSystems().CreateSystem<TestSystem>();

		SystemsRegistry::GetInstance().RegisterSystem<TestSystem2>();
		scene->GetGameplayWorld()->GetSystems().CreateSystem<TestSystem2>();

		scene->GetGameplayWorld()->GetHierarchy().MakeGlobalTransformsCurrent();
	}
}