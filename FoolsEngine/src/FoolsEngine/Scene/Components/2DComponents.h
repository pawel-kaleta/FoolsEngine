#pragma once

#include "FoolsEngine/Scene/Component.h"

#include "FoolsEngine\Renderer\8 - Render\Renderer2D.h"

namespace fe
{
	struct CTile final : SpatialComponent
	{
		Renderer2D::Quad Tile;

		FE_COMPONENT_SETUP(CTile, "Tile");
		virtual void DrawInspectorWidget(BaseEntity entity) override;
		virtual void Serialize(YAML::Emitter& emitter) override;
		virtual void Deserialize(YAML::Node& data) override;
	};

	struct CSprite final : SpatialComponent
	{
		Renderer2D::Quad Sprite;

		FE_COMPONENT_SETUP(CSprite, "Sprite");
		virtual void DrawInspectorWidget(BaseEntity entity) override;
		virtual void Serialize(YAML::Emitter& emitter) override;
		virtual void Deserialize(YAML::Node& data) override;
	};
}