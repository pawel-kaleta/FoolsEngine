#pragma once

#include <glm\glm.hpp>
#include "Transform.h"

namespace fe::Math
{
	bool DecomposeTransform(const glm::mat4& matrix, Transform& outTransform);
}