#pragma once

#include <glm\glm.hpp>
#include "FoolsEngine\Renderer\3 - Representation\Transform.h"

namespace fe::Math
{
	bool DecomposeTransform(const glm::mat4& matrix, Transform& outTransform);
}