#pragma once

#include <glm/glm.hpp>

namespace fe
{
	struct Transform;
	namespace Math
	{
		bool DecomposeTransform(const glm::mat4& matrix, Transform& outTransform);
	}
}