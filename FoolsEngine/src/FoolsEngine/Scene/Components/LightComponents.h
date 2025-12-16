#pragma once

#include "FoolsEngine\Scene\Component.h"

#include "FoolsEngine\Renderer\4 - Representation\Lights.h"

namespace fe
{
	struct CDirectionalLight final : DataComponent
	{
		DirectionalLight DirectionalLight;
		bool IsPrimary;

		FE_COMPONENT_SETUP(CDirectionalLight, "Directional Light");
		virtual void DrawInspectorWidget(BaseEntity entity) override;
		virtual void Serialize(YAML::Emitter& emitter) override;
		virtual void Deserialize(YAML::Node& data) override;
	};
}