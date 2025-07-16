#pragma once

#include <FoolsEngine.h>

namespace fe
{
	struct CMovement : DataComponent
	{
		struct TargetMovement {
			float MoveSpeed = 0.5f;
			float RotationSpeed = 80.0f;

			Transform CalculateNewTransform(Transform transform) const
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
			ImGui::DragFloat("MoveSpeed", &Movement.MoveSpeed, 0.01f);
			ImGui::DragFloat("RotationSpeed", &Movement.RotationSpeed, 0.10f);
		}

		virtual void Serialize(YAML::Emitter& emitter) override
		{
			emitter << YAML::Key << "MoveSpeed" << YAML::Value << Movement.MoveSpeed;
			emitter << YAML::Key << "RotationSpeed" << YAML::Value << Movement.RotationSpeed;
		}

		virtual void Deserialize(YAML::Node& data) override
		{
			Movement.MoveSpeed = data["MoveSpeed"].as<float>();
			Movement.RotationSpeed = data["RotationSpeed"].as<float>();
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
			RegisterForUpdate<SimulationStage::PrePhysics>(10);
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

	void RegisterGameplayTypes()
	{
		static bool once = false;

		if (once)
			return;

		BehaviorsRegistry::RegisterBehavior<PlayerMovementBehavior>();
		ComponentTypesRegistry::RegisterDataComponent<CMovement>();

		once = true;
	}
}