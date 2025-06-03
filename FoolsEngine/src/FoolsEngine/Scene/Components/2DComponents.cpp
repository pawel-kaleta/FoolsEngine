#include "FE_pch.h"
#include "2DComponents.h"

#include "FoolsEngine\Scene\BaseEntity.h"
#include "FoolsEngine\Assets\Serialization\YAML.h"
#include "FoolsEngine\Renderer\9 - Integration\Renderer.h"

#include <glm\gtc\type_ptr.hpp>

namespace fe
{
	void CTile::DrawInspectorWidget(BaseEntity entity)
	{
		std::string nameTag = "Texture";
		DrawAssetHandle<Texture2D>(Tile.Texture, nameTag);

		if (Tile.Texture == Renderer::BaseAssets.Textures.FlatWhite)
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
		std::string nameTag = "Texture";
		DrawAssetHandle<Texture2D>(Sprite.Texture, nameTag);

		if (Sprite.Texture == Renderer::BaseAssets.Textures.FlatWhite)
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
}