#include "FE_pch.h"

#include "Component.h"

#include "BaseEntity.h"
#include "FoolsEngine\Scene\GameplayWorld\Entity.h"
#include "World.h"
#include "Scene.h"

namespace fe
{
	void DataComponent::DrawInspectorWidget(BaseEntity entity) { }

	void CCamera::DrawInspectorWidget(BaseEntity entity)
	{
		auto* scene = entity.GetWorld()->GetScene();

		bool primary = entity.ID() == scene->GetEntityWithPrimaryCamera().ID();

		if (ImGui::Checkbox("Primary", &primary))
			if (primary)
				scene->SetPrimaryCameraEntity(Entity(entity));

		constexpr char* projectionTypeStrings[] = { "Orthographic", "Perspective" };
		const char* currentProjectionTypeString = projectionTypeStrings[(int)Camera.GetProjectionType()];

		if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
		{
			for (int i = 0; i < 2; i++)
			{
				bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
				if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
				{
					currentProjectionTypeString = projectionTypeStrings[i];
					Camera.SetProjectionType((Camera::ProjectionType)i);
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

	void CTile::DrawInspectorWidget(BaseEntity entity)
	{
		auto& textures = TextureLibrary::GetAll();
		auto flat_color_texture = TextureLibrary::Get("Base2DTexture");

		auto item_current = Tile.Texture;
		const char* combo_preview_value = item_current->GetID() == flat_color_texture->GetID() ? "None" : item_current->GetName().c_str();
		if (ImGui::BeginCombo("Texture", combo_preview_value))
		{
			bool is_selected = (item_current->GetID() == flat_color_texture->GetID());

			if (ImGui::Selectable("None", is_selected))
				item_current = flat_color_texture;

			for (auto it = textures.begin(); it != textures.end(); ++it)
			{
				if (it->second->GetFormat() != TextureData::Format::RGB)
					continue;

				if (it->second->GetID() == flat_color_texture->GetID())
					continue;

				is_selected = (item_current->GetID() == it->second.get()->GetID());
				if (ImGui::Selectable(it->first.c_str(), is_selected))
					item_current = it->second;

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		Tile.Texture = item_current;

		if (item_current->GetID() == flat_color_texture->GetID())
		{
			ImGui::ColorEdit3("Color", glm::value_ptr(Tile.Color));
		}
		else
		{
			ImGui::DragFloat("Tiling Factor", &Tile.TextureTilingFactor, 0.1f);

			ImGui::ColorEdit3("Tint Color", glm::value_ptr(Tile.Color));
		}
	}

	void CSprite::DrawInspectorWidget(BaseEntity entity)
	{
		auto& textures = TextureLibrary::GetAll();
		auto flat_color_texture = TextureLibrary::Get("Base2DTexture");

		auto item_current = Sprite.Texture;
		const char* combo_preview_value = item_current->GetID() == flat_color_texture->GetID() ? "None" : item_current->GetName().c_str();
		if (ImGui::BeginCombo("Texture", combo_preview_value))
		{
			bool is_selected = (item_current->GetID() == flat_color_texture->GetID());

			if (ImGui::Selectable("None", is_selected))
				item_current = flat_color_texture;

			for (auto it = textures.begin(); it != textures.end(); ++it)
			{
				if (it->second->GetID() == flat_color_texture->GetID())
					continue;

				is_selected = (item_current->GetID() == it->second.get()->GetID());
				if (ImGui::Selectable(it->first.c_str(), is_selected))
					item_current = it->second;

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		Sprite.Texture = item_current;

		if (item_current->GetID() == flat_color_texture->GetID())
		{
			ImGui::ColorEdit4("Color", glm::value_ptr(Sprite.Color));
		}
		else
		{
			ImGui::DragFloat("Tiling Factor", &Sprite.TextureTilingFactor, 0.1f);

			ImGui::ColorEdit4("Tint Color", glm::value_ptr(Sprite.Color));
		}
	}
}