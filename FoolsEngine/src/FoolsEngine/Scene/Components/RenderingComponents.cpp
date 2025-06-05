#include "FE_pch.h"
#include "RenderingComponents.h"

#include "FoolsEngine\Scene\BaseEntity.h"
#include "FoolsEngine\Assets\Serialization\YAML.h"

#include "FoolsEngine\Scene\GameplayWorld\Entity.h"
#include "FoolsEngine\Scene\GameplayWorld\GameplayWorld.h"

namespace fe
{
	void CCamera::DrawInspectorWidget(BaseEntity entity)
	{
		auto* world = entity.GetWorld();

		if (world->IsGameplayWorld())
		{
			if (entity.ID() == ((GameplayWorld*)world)->GetEntityWithPrimaryCamera().ID())
			{
				IsPrimary = true;
			}

			if (ImGui::Checkbox("Primary", &IsPrimary))
				if (IsPrimary)
					((GameplayWorld*)world)->SetPrimaryCameraEntity(Entity(entity));
		}

		constexpr const char* projectionTypeStrings[] = { "Orthographic", "Perspective" };
		const char* currentProjectionTypeString = projectionTypeStrings[Camera.GetProjectionType().ToInt()];

		if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
		{
			for (int i = 0; i < 2; i++)
			{
				bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
				if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
				{
					currentProjectionTypeString = projectionTypeStrings[i];
					Camera::ProjectionType type;
					type.FromInt(i);
					Camera.SetProjectionType(type);
				}

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		if (Camera.GetProjectionType() == Camera::ProjectionType::Perspective)
		{
			float verticalFov = glm::degrees(Camera.GetPerspectiveFOV());
			if (ImGui::DragFloat("Field of View", &verticalFov))
				Camera.SetPerspectiveFOV(glm::radians(verticalFov));

			float orthoNear = Camera.GetPerspectiveNearClip();
			if (ImGui::DragFloat("Near Clip", &orthoNear))
				Camera.SetPerspectiveNearClip(orthoNear);

			float orthoFar = Camera.GetPerspectiveFarClip();
			if (ImGui::DragFloat("Far Clip", &orthoFar))
				Camera.SetPerspectiveFarClip(orthoFar);
		}
		else
		{
			float zoom = Camera.GetOrthographicZoom();
			if (ImGui::DragFloat("Zoom", &zoom))
				Camera.SetOrthographicZoom(zoom);

			float orthoNear = Camera.GetOrthographicNearClip();
			if (ImGui::DragFloat("Near Clip", &orthoNear))
				Camera.SetOrthographicNearClip(orthoNear);

			float orthoFar = Camera.GetOrthographicFarClip();
			if (ImGui::DragFloat("Far Clip", &orthoFar))
				Camera.SetOrthographicFarClip(orthoFar);
		}
	}

	void CCamera::Serialize(YAML::Emitter& emitter)
	{
		emitter << YAML::Key << "IsPrimary" << YAML::Value << IsPrimary;

		emitter << YAML::Key << "ProjectionType" << YAML::Value << Camera.GetProjectionType().ToString();

		emitter << YAML::Key << "PerspectiveNear" << YAML::Value << Camera.GetPerspectiveNearClip();
		emitter << YAML::Key << "PerspectiveFar" << YAML::Value << Camera.GetPerspectiveFarClip();
		emitter << YAML::Key << "PerspectiveFOV" << YAML::Value << Camera.GetPerspectiveFOV();

		emitter << YAML::Key << "OrthographicNear" << YAML::Value << Camera.GetOrthographicNearClip();
		emitter << YAML::Key << "OrthographicFar" << YAML::Value << Camera.GetOrthographicFarClip();
		emitter << YAML::Key << "OrthographicZoom" << YAML::Value << Camera.GetOrthographicZoom();
	}

	void CCamera::Deserialize(YAML::Node& data)
	{
		IsPrimary = data["IsPrimary"].as<bool>();

		Camera::ProjectionType projection;
		projection.FromString(data["ProjectionType"].as<std::string>());
		Camera.SetProjectionType(projection);

		Camera.SetPerspectiveNearClip(data["PerspectiveNear"].as<float>());
		Camera.SetPerspectiveFarClip(data["PerspectiveFar"].as<float>());
		Camera.SetPerspectiveFOV(data["PerspectiveFOV"].as<float>());

		Camera.SetOrthographicNearClip(data["OrthographicNear"].as<float>());
		Camera.SetOrthographicFarClip(data["OrthographicFar"].as<float>());
		Camera.SetOrthographicZoom(data["OrthographicZoom"].as<float>());
	}
}