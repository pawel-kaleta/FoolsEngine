#include "FE_pch.h"

#include "Component.h"

#include "BaseEntity.h"
#include "FoolsEngine\Scene\GameplayWorld\Entity.h"
#include "World.h"
#include "Scene.h"
#include "FoolsEngine/Platform/FileDialogs.h"

#include "SceneSerializer.h"

namespace fe
{
	void DataComponent::DrawInspectorWidget(BaseEntity entity)
	{
		FE_LOG_CORE_ERROR("UI widget drawing of {0} not implemented!", this->GetComponentName());
	}

	void DataComponent::Serialize(YAML::Emitter& emitter)
	{
		FE_LOG_CORE_ERROR("{0} serialization not implemented!", this->GetComponentName());
	}

	void DataComponent::Deserialize(YAML::Node& data)
	{
		FE_LOG_CORE_ERROR("{0} deserialization not implemented!", this->GetComponentName());
	}

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

	void CCamera::Serialize(YAML::Emitter& emitter)
	{
		emitter << YAML::Key << "IsPrimary"        << YAML::Value << IsPrimary;

		emitter << YAML::Key << "ProjectionType"   << YAML::Value << Camera.GetProjectionType();

		emitter << YAML::Key << "PerspectiveNear"  << YAML::Value << Camera.GetPerspectiveNearClip();
		emitter << YAML::Key << "PerspectiveFar"   << YAML::Value << Camera.GetPerspectiveFarClip();
		emitter << YAML::Key << "PerspectiveFOV"   << YAML::Value << Camera.GetPerspectiveFOV();

		emitter << YAML::Key << "OrthographicNear" << YAML::Value << Camera.GetOrthographicNearClip();
		emitter << YAML::Key << "OrthographicFar"  << YAML::Value << Camera.GetOrthographicFarClip();
		emitter << YAML::Key << "OrthographicZoom" << YAML::Value << Camera.GetOrthographicZoom();
	}

	void CCamera::Deserialize(YAML::Node& data)
	{
		IsPrimary = data["IsPrimary"].as<bool>();

		Camera.SetProjectionType((Camera::ProjectionType)data["ProjectionType"].as<int>());
			   
		Camera.SetPerspectiveNearClip( data["PerspectiveNear" ].as<float>());
		Camera.SetPerspectiveFarClip(  data["PerspectiveFar"  ].as<float>());
		Camera.SetPerspectiveFOV(      data["PerspectiveFOV"  ].as<float>());
								   
		Camera.SetOrthographicNearClip(data["OrthographicNear"].as<float>());
		Camera.SetOrthographicFarClip( data["OrthographicFar" ].as<float>());
		Camera.SetOrthographicZoom(    data["OrthographicZoom"].as<float>());
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
			if (ImGui::Selectable("Add..."))
			{
				const std::filesystem::path newTextureFilepath = FileDialogs::OpenFile("(*.*)\0*.*\0");
				if (!newTextureFilepath.empty())
				{
					const std::string textureName = FileNameFromFilepath(newTextureFilepath.string());
					if (!TextureLibrary::Exist(textureName))
					{
						Ref<Texture> texture = Texture2D::Create(newTextureFilepath.string());
						TextureLibrary::Add(texture);
					}
					item_current = TextureLibrary::Get(textureName);
				}
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

	void CTile::Serialize(YAML::Emitter& emitter)
	{
		emitter << YAML::Key << "Color"   << YAML::Value << Tile.Color;
		emitter << YAML::Key << "Texture" << YAML::Value << Tile.Texture->GetFilePath().c_str();
		emitter << YAML::Key << "Tiling"  << YAML::Value << Tile.TextureTilingFactor;
	}

	void CTile::Deserialize(YAML::Node& data)
	{
		Tile.Color = data["Color"].as<glm::vec4>();

		const std::string textureFilePath = data["Texture"].as<std::string>();
		std::string textureName;
		if (textureFilePath.empty())
			textureName = "Base2DTexture";
		else
			textureName = FileNameFromFilepath(textureFilePath);

		if (!TextureLibrary::Exist(textureName))
			TextureLibrary::Add(Texture2D::Create(textureFilePath));

		Tile.Texture = TextureLibrary::Get(textureName);

		Tile.TextureTilingFactor = data["Tiling"].as<float>();
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

			if (ImGui::Selectable("Add..."))
			{
				const std::filesystem::path newTextureFilepath = FileDialogs::OpenFile("(*.*)\0*.*\0");
				if (!newTextureFilepath.empty())
				{
					const std::string textureName = FileNameFromFilepath(newTextureFilepath.string());
					if (!TextureLibrary::Exist(textureName))
					{
						Ref<Texture> texture = Texture2D::Create(newTextureFilepath.string());
						TextureLibrary::Add(texture);
					}
					item_current = TextureLibrary::Get(textureName);
				}
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

	void CSprite::Serialize(YAML::Emitter& emitter)
	{
		emitter << YAML::Key << "Color"   << YAML::Value << Sprite.Color;
		emitter << YAML::Key << "Texture" << YAML::Value << Sprite.Texture->GetFilePath().c_str();
		emitter << YAML::Key << "Tiling"  << YAML::Value << Sprite.TextureTilingFactor;
	}

	void CSprite::Deserialize(YAML::Node& data)
	{
		Sprite.Color = data["Color"].as<glm::vec4>();

		const std::string textureFilePath = data["Texture"].as<std::string>();
		std::string textureName;
		if (textureFilePath.empty())
			textureName = "Base2DTexture";
		else
			textureName = FileNameFromFilepath(textureFilePath);

		if (!TextureLibrary::Exist(textureName))
			TextureLibrary::Add(Texture2D::Create(textureFilePath));

		Sprite.Texture = TextureLibrary::Get(textureName);

		Sprite.TextureTilingFactor = data["Tiling"].as<float>();
	}
}