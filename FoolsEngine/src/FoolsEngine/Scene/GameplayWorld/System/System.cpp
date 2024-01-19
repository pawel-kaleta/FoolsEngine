#include "FE_pch.h"

#include "System.h"
#include "FoolsEngine\Scene\GameplayWorld\GameplayWorld.h"

namespace fe
{
	void System::Deserialize(YAML::Node& data, GameplayWorld* world)
	{
		FE_LOG_CORE_ERROR("{0} serialization not implemented!", GetSystemName());
	}
}