#pragma once

#include "YAML.h"

#include "FoolsEngine\Renderer\1 - Description\Data.h"

namespace fe
{
	void EmitGPUDataType(YAML::Emitter& emitter, char* dataPtr, const Description::Data::Type& type);
	bool LoadGPUDataType(const YAML::Node& node, char* dataPtr, Description::Data::Type type);
}