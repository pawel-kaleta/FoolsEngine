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

		glm::mat4 GetTransform() const
		{
			auto trans = glm::translate(glm::mat4(1.0f), Shift);
			auto rot = glm::toMat4(glm::quat(glm::radians(Rotation)));
			auto scale = glm::scale(glm::mat4(1.0f), Scale);
			return trans * rot * scale;
		}

		operator glm::mat4() { return GetTransform(); }
		operator const glm::mat4() const { return GetTransform(); }

	};

	// used for calculating global/local transform of a child using parents transform 
	// child.global = parent.global + child.local
	// child.local = child.global - parent.global
	inline Transform operator+ (const Transform& a, const Transform& b)
	{
		Transform newTransform;
		newTransform.Shift = a.Shift + a.Scale * glm::rotate(glm::quat(glm::radians(a.Rotation)), b.Shift);
		newTransform.Rotation = a.Rotation + b.Rotation;
		newTransform.Scale = a.Scale * b.Scale;
		return newTransform;
	}
	inline Transform operator- (const Transform& a, const Transform& b)
	{
		Transform newTransform;
		newTransform.Shift = glm::rotate(glm::quat(-1.0f * b.Rotation), (a.Shift - b.Shift));
		newTransform.Rotation = a.Rotation - b.Rotation;
		newTransform.Scale = a.Scale / b.Scale;
		return newTransform;
	}
	inline bool operator==(const Transform& a, const Transform& b)
	{
		return a.Shift == b.Shift && a.Rotation == b.Rotation && a.Scale == b.Scale;
	}
}