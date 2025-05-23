#pragma once

namespace YAML
{
	class Emitter;
	class Node;
}

namespace fe
{
	enum AssetType;

	class AssetSerializer
	{
	public:
		static void Serialize(YAML::Emitter& emitter);
		static bool Deserialize(YAML::Node& node);
	private:
		static const char* AssetTypeName(AssetType type);
	};
}