#include "FE_pch.h"
#include "Transform.h"

namespace fe
{
	glm::mat4 Transform::GetMatrix() const
	{
		auto trans = glm::translate(glm::mat4(1.0f), Shift);
		auto rot = glm::toMat4(glm::quat(glm::radians(Rotation)));
		auto scale = glm::scale(glm::mat4(1.0f), Scale);
		return trans * rot * scale;
	}

	Transform operator+ (const Transform& a, const Transform& b)
	{
		Transform newTransform;
		newTransform.Shift = a.Shift + a.Scale * glm::rotate(glm::quat(glm::radians(a.Rotation)), b.Shift);
		newTransform.Rotation = a.Rotation + b.Rotation;
		newTransform.Scale = a.Scale * b.Scale;
		return newTransform;
	}

	Transform operator- (const Transform& a, const Transform& b)
	{
		Transform newTransform;
		newTransform.Shift = glm::rotate(glm::quat(-1.0f * b.Rotation), (a.Shift - b.Shift));
		newTransform.Rotation = a.Rotation - b.Rotation;
		newTransform.Scale = a.Scale / b.Scale;
		return newTransform;
	}

	bool operator==(const Transform& a, const Transform& b)
	{
		return a.Shift == b.Shift && a.Rotation == b.Rotation && a.Scale == b.Scale;
	}
}