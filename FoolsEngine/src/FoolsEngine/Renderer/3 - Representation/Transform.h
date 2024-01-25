#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\quaternion.hpp>
#include <glm\gtc\quaternion.hpp>

namespace fe
{
	struct Transform
	{
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

		glm::mat4 GetTransform() const
		{
			auto trans = glm::translate(glm::mat4(1.0f), Position);
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
		newTransform.Position = a.Position + a.Scale * glm::rotate(glm::quat(glm::radians(a.Rotation)), b.Position);
		newTransform.Rotation = a.Rotation + b.Rotation;
		newTransform.Scale = a.Scale * b.Scale;
		return newTransform;
	}
	inline Transform operator- (const Transform& a, const Transform& b)
	{
		Transform newTransform;
		newTransform.Position = glm::rotate(glm::quat(-1.0f * b.Rotation), (a.Position - b.Position));
		newTransform.Rotation = a.Rotation - b.Rotation;
		newTransform.Scale = a.Scale / b.Scale;
		return newTransform;
	}
	inline bool operator==(const Transform& a, const Transform& b)
	{
		return a.Position == b.Position && a.Rotation == b.Rotation && a.Scale == b.Scale;
	}
}