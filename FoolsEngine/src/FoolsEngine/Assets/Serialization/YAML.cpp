#include "YAML.h"

#include "FoolsEngine/Scene/GameplayWorld/Entity.h"

namespace fe
{
	YAML::Emitter& operator<<(YAML::Emitter& out, const Entity& entity)
	{
		if (entity)
			out << entity.Get<CUUID>().UUID;
		else
			out << UUID(0);

		return out;
	}
}