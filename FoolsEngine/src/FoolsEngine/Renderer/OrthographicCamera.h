#pragma once

#include <glm\glm.hpp>

namespace fe
{
	class OrthographicCamera
	{
	public:
		//OrtographicCamera() = default;
		OrthographicCamera(float left, float right, float bottom, float top);

		void SetProjection(float left, float right, float bottom, float top);

		void SetPosition(const glm::vec3& position) { m_Position = position; m_VMatrixInvalid = true; }
		void SetRotation(const float& rotation) { m_Rotation = rotation; m_VMatrixInvalid = true; }
		const glm::vec3& GetPosition() const { return m_Position; }
		const float& GetRotation() const { return m_Rotation; }

		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() { if (m_VMatrixInvalid) RecalculateViewMatrix(); return m_ViewMatrix; }
		const glm::mat4& GetViewProjectionMatrix() { if (m_VMatrixInvalid) RecalculateViewMatrix(); return m_ViewProjectionMatrix; }

	private:
		glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
		glm::mat4 m_ViewProjectionMatrix = glm::mat4(1.0f);

		glm::vec3 m_Position = { 0.0f, 0.0f, 1.0f };
		float m_Rotation = 0.0f;

		bool m_VMatrixInvalid = true;

		void RecalculateViewMatrix();
	};
}