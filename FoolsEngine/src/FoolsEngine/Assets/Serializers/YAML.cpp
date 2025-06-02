#include "FE_pch.h"
#include "YAML.h"

#include "FoolsEngine\Scene\GameplayWorld\Entity.h"

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

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec1& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}
}