#pragma once

#include <yaml-cpp\yaml.h>

#include "FoolsEngine\Assets\Asset.h"

namespace fe
{
	class AssetSerializer
	{
	public:
		static void Serialize(YAML::Emitter& emitter);
		static bool Deserialize(YAML::Node& node);
	private:
		static const char* AssetTypeName(AssetType type);
	};
}