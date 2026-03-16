#pragma once

#include <glm/glm.hpp>

namespace fe
{
#define FE_RGBA(r, g, b, a) glm::vec4(float(r)/255.f, float(g)/255.f, float(b)/255.f, float(a))

}