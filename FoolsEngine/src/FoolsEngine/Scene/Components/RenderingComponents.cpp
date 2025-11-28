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

		if (world->m_IsGameplayWorld)
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
		const char* currentProjectionTypeString = projectionTypeStrings[Camera.m_ProjectionType.ToInt()];

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

		if (Camera.m_ProjectionType == Camera::ProjectionType::Perspective)
		{
			const auto& data = Camera.m_PerspectiveData;
			float fov = glm::degrees(data.m_FOV);
			if (ImGui::DragFloat("Field of View", &fov))
				Camera.SetPerspectiveFOV(glm::radians(fov));

			float near_clip = data.m_NearClip;
			if (ImGui::DragFloat("Near Clip", &near_clip))
				Camera.SetPerspectiveNearClip(near_clip);

			float far_clip = data.m_NearClip;
			if (ImGui::DragFloat("Far Clip", &far_clip))
				Camera.SetPerspectiveFarClip(far_clip);
		}
		else
		{
			const auto& data = Camera.m_OrthographicData;

			float zoom = data.m_Zoom;
			if (ImGui::DragFloat("Zoom", &zoom))
				Camera.SetOrthographicZoom(zoom);

			float near_clip = data.m_NearClip;
			if (ImGui::DragFloat("Near Clip", &near_clip))
				Camera.SetOrthographicNearClip(near_clip);

			float far_clip = data.m_FarClip;
			if (ImGui::DragFloat("Far Clip", &far_clip))
				Camera.SetOrthographicFarClip(far_clip);
		}
	}

	void CCamera::Serialize(YAML::Emitter& emitter)
	{
		emitter << YAML::Key << "IsPrimary" << YAML::Value << IsPrimary;

		emitter << YAML::Key << "ProjectionType" << YAML::Value << Camera.m_ProjectionType.ToConstCharPtr();

		const auto& data_pers = Camera.m_PerspectiveData;
		emitter << YAML::Key << "PerspectiveNear" << YAML::Value << data_pers.m_NearClip;
		emitter << YAML::Key << "PerspectiveFar" << YAML::Value << data_pers.m_FarClip;
		emitter << YAML::Key << "PerspectiveFOV" << YAML::Value << data_pers.m_FOV;

		const auto& data_orto = Camera.m_OrthographicData;
		emitter << YAML::Key << "OrthographicNear" << YAML::Value << data_orto.m_NearClip;
		emitter << YAML::Key << "OrthographicFar" << YAML::Value << data_orto.m_FarClip;
		emitter << YAML::Key << "OrthographicZoom" << YAML::Value << data_orto.m_Zoom;
	}

	void CCamera::Deserialize(YAML::Node& data)
	{
		IsPrimary = data["IsPrimary"].as<bool>();

		auto& data_pers = Camera.m_PerspectiveData;
		auto& data_orto = Camera.m_OrthographicData;

		data_pers.m_NearClip = data["PerspectiveNear"].as<float>();
		data_pers.m_FarClip = data["PerspectiveFar"].as<float>();
		data_pers.m_FOV = data["PerspectiveFOV"].as<float>();

		data_orto.m_NearClip = data["OrthographicNear"].as<float>();
		data_orto.m_FarClip = data["OrthographicFar"].as<float>();
		data_orto.m_Zoom = data["OrthographicZoom"].as<float>();

		Camera::ProjectionType projection;
		projection.FromString(data["ProjectionType"].as<std::string>());
		Camera.SetProjectionType(projection);
	}
}