#include "2DComponents.h"

#include "FoolsEngine/Scene/BaseEntity.h"

#include "FoolsEngine/Assets/Serialization/YAML.h"

#include "FoolsEngine/Renderer/5 - Integration/Director.h"

#include <glm/gtc/type_ptr.hpp>

namespace fe
{
	void CTile::DrawInspectorWidget(BaseEntity entity)
	{
		DrawAssetHandle<Texture2D>(Tile.Texture);
		ImGui::SeparatorText("Quad");
		if (Tile.Texture == Render::Integration::BaseAssets->Textures.FlatWhite)
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
		emitter << YAML::Key << "Color" << YAML::Value << Tile.Color;
		emitter << YAML::Key << "Texture" << YAML::Value << Tile.Texture;
		emitter << YAML::Key << "Tiling" << YAML::Value << Tile.TextureTilingFactor;
	}

	void CTile::Deserialize(YAML::Node& data)
	{
		Tile.Color = data["Color"].as<glm::vec4>();
		Tile.Texture = data["Texture"].as<AssetHandle<Texture2D>>();
		Tile.TextureTilingFactor = data["Tiling"].as<float>();
	}

	void CSprite::DrawInspectorWidget(BaseEntity entity)
	{
		DrawAssetHandle<Texture2D>(Sprite.Texture);
		ImGui::SeparatorText("Quad");
		if (Sprite.Texture == Render::Integration::BaseAssets->Textures.FlatWhite)
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
		emitter << YAML::Key << "Color" << YAML::Value << Sprite.Color;
		emitter << YAML::Key << "Texture" << YAML::Value << Sprite.Texture;
		emitter << YAML::Key << "Tiling" << YAML::Value << Sprite.TextureTilingFactor;
	}

	void CSprite::Deserialize(YAML::Node& data)
	{
		Sprite.Color = data["Color"].as<glm::vec4>();
		Sprite.Texture = data["Texture"].as<AssetHandle<Texture2D>>();
		Sprite.TextureTilingFactor = data["Tiling"].as<float>();
	}

	void CSpriteBlend::DrawInspectorWidget(BaseEntity entity)
	{
		DrawAssetHandle<Texture2D>(SpriteBlend.Texture);
		ImGui::SeparatorText("Quad");
		if (SpriteBlend.Texture == Render::Integration::BaseAssets->Textures.FlatWhite)
		{
			ImGui::ColorEdit4("Color", glm::value_ptr(SpriteBlend.Color));
		}
		else
		{
			ImGui::DragFloat("Tiling Factor", &SpriteBlend.TextureTilingFactor, 0.1f);

			ImGui::ColorEdit4("Tint Color", glm::value_ptr(SpriteBlend.Color));
		}
	}

	void CSpriteBlend::Serialize(YAML::Emitter& emitter)
	{
		emitter << YAML::Key << "Color" << YAML::Value << SpriteBlend.Color;
		emitter << YAML::Key << "Texture" << YAML::Value << SpriteBlend.Texture;
		emitter << YAML::Key << "Tiling" << YAML::Value << SpriteBlend.TextureTilingFactor;
	}

	void CSpriteBlend::Deserialize(YAML::Node& data)
	{
		SpriteBlend.Color = data["Color"].as<glm::vec4>();
		SpriteBlend.Texture = data["Texture"].as<AssetHandle<Texture2D>>();
		SpriteBlend.TextureTilingFactor = data["Tiling"].as<float>();
	}
}