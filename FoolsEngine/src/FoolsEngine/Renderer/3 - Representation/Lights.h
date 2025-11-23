#pragma once

#include <glm\glm.hpp>

#include "FoolsEngine\Utils\DeclareEnum.h"

namespace fe
{
	struct Light
	{
		FE_DECLARE_ENUM(Type, Directional, Spot, Point);

		virtual Type GetType() = 0;
	};

	struct DirectionalLight final : Light
	{
		glm::vec3 Direction;
		glm::vec3 Color;
		float Intensity;

		Type GetTypeStatic() { return Type::Directional; }
		virtual Type GetType() override final { return Type::Directional; }
	};

	struct PointLight final : Light
	{
		glm::vec3 Position;
		glm::vec3 Color;
		float Intensity;
		float Range;

		Type GetTypeStatic() { return Type::Point; }
		virtual Type GetType() override final { return Type::Point; }
	};
}