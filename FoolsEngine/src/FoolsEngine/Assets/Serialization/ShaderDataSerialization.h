#pragma once

#include "YAML.h"

#include "FoolsEngine\Renderer\1 - Primitives\ShaderData.h"
#include "FoolsEngine\Renderer\1 - Primitives\Uniform.h"

namespace fe
{
	void EmitShaderDataType(YAML::Emitter& emitter, char* dataPtr, const ShaderData::Type& type);
	bool LoadShaderDataType(const YAML::Node& node, char* dataPtr, ShaderData::Type type);
}