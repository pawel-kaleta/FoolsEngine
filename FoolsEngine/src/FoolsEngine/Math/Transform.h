#pragma once

#include "FoolsEngine\Scene\ECS.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\quaternion.hpp>
#include <glm\gtc\quaternion.hpp>

namespace fe
{
	using Shift    = glm::vec3;
	using Rotation = glm::vec3;
	using Scale    = glm::vec3;

	struct Position
	{
		Shift    Shift  = { 0.0f, 0.0f, 0.0f };
		EntityID Origin = NullEntityID;
	};
	struct Orientation
	{
		Rotation Rotation = { 0.0f, 0.0f, 0.0f };
		EntityID Origin   = NullEntityID;
	};
	struct Size
	{
		Scale    Scale  = { 1.0f, 1.0f, 1.0f };
		EntityID Origin = NullEntityID;
	};

	// TO DO: operations on transform components (eg. Position + Shift = Position, Orientation - Orientation = Rotation)

	struct Transform
	{
		Shift    Shift    = { 0.0f, 0.0f, 0.0f };
		Rotation Rotation = { 0.0f, 0.0f, 0.0f };
		Scale    Scale    = { 1.0f, 1.0f, 1.0f };

		glm::mat4 GetMatrix() const;

		operator glm::mat4() { return GetMatrix(); }
		operator const glm::mat4() const { return GetMatrix(); }

	};

	// used for calculating global/local transform of a child using parents transform 
	// child.global = parent.global + child.local
	// child.local = child.global - parent.global
	Transform operator+ (const Transform& a, const Transform& b);
	Transform operator- (const Transform& a, const Transform& b);
	bool operator==(const Transform& a, const Transform& b);
}