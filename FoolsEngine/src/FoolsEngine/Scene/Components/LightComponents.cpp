#include "FE_pch.h"
#include "LightComponents.h"

#include "FoolsEngine/Scene/BaseEntity.h"
#include "FoolsEngine/Scene/GameplayWorld/Entity.h"
#include "FoolsEngine/Scene/GameplayWorld/GameplayWorld.h"

#include "FoolsEngine/Assets/Serialization/YAML.h"

#include <glm/gtc/type_ptr.hpp>

namespace fe
{
	void CDirectionalLight::DrawInspectorWidget(BaseEntity entity)
	{
		ImGui::DragFloat3("Direction", glm::value_ptr(DirectionalLight.Direction), 0.01f, -1.0f, 1.0f);
		ImGui::ColorEdit3("Color", glm::value_ptr(DirectionalLight.Color));
		ImGui::DragFloat("Intensity", &DirectionalLight.Intensity, 0.01f, 0.0f, 4.0f);

		auto* world = entity.GetWorld();

		if (world->m_IsGameplayWorld)
		{
			if (entity.ID() == ((GameplayWorld*)world)->GetEntityWithPrimaryDirectionalLight().ID())
			{
				IsPrimary = true;
			}

			if (ImGui::Checkbox("Primary", &IsPrimary))
				if (IsPrimary)
					((GameplayWorld*)world)->SetPrimaryDirectionalLightEntity(Entity(entity));
		}
	}

	void CDirectionalLight::Serialize(YAML::Emitter& emitter)
	{
		emitter << YAML::Key << "IsPrimary" << YAML::Value << IsPrimary;
		emitter << YAML::Key << "Direction" << YAML::Value << DirectionalLight.Direction;
		emitter << YAML::Key << "Color" << YAML::Value << DirectionalLight.Color;
		emitter << YAML::Key << "Intensity" << YAML::Value << DirectionalLight.Intensity;
	}

	void CDirectionalLight::Deserialize(YAML::Node& data)
	{
		IsPrimary = data["IsPrimary"].as<bool>();

		DirectionalLight.Direction = data["Direction"].as<glm::vec3>();
		DirectionalLight.Color = data["Color"].as<glm::vec3>();
		DirectionalLight.Intensity = data["Intensity"].as<float>();
	}
}