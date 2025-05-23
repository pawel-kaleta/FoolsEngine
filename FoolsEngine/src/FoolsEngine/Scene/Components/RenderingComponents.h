#pragma once

#include "FoolsEngine/Scene/Component.h"

#include "FoolsEngine\Renderer\3 - Representation\Camera.h"

namespace fe
{
	struct CCamera final : SpatialComponent
	{
		Camera Camera;
		bool IsPrimary = false;

		FE_COMPONENT_SETUP(CCamera, "Camera");
		virtual void DrawInspectorWidget(BaseEntity entity) override;
		virtual void Serialize(YAML::Emitter& emitter) override;
		virtual void Deserialize(YAML::Node& data) override;
	};
}