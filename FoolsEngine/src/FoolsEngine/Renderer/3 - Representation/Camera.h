#pragma once

#include <glm\glm.hpp>

namespace fe
{
	class Camera
	{
	public:
		enum ProjectionType
		{
			Orthographic,
			Perspective
		};

		Camera() { CalculateProjection(); }
		~Camera() = default;

		glm::mat4 GetProjectionMatrix() const { return m_Projection; };
		operator glm::mat4() { return m_Projection; }
		operator const glm::mat4() const { return m_Projection; }

		void SetOrthographic(float zoom, float nearClip, float farClip);
		void SetPerspective(float verticalFOV, float nearClip, float farClip);

		void SetViewportSize(uint32_t width, uint32_t hight) { m_AspectRatio = (float)width / (float)hight; CalculateProjection(); }

		//radians
		float GetPerspectiveFOV() const { return m_PerspectiveFOV; }
		//radians
		void SetPerspectiveFOV(float FOV) { m_PerspectiveFOV = FOV; CalculateProjection(); }
		float GetPerspectiveNearClip() const { return m_PerspectiveNearClip; }
		void SetPerspectiveNearClip(float nearClip) { m_PerspectiveNearClip = nearClip; CalculateProjection(); }
		float GetPerspectiveFarClip() const { return m_PerspectiveFarClip; }
		void SetPerspectiveFarClip(float farClip) { m_PerspectiveFarClip = farClip; CalculateProjection(); }

		float GetOrthographicZoom() const { return m_OrthographicZoom; }
		void SetOrthographicZoom(float zoom) { m_OrthographicZoom = zoom; CalculateProjection(); }
		float GetOrthographicNearClip() const { return m_OrthographicNearClip; }
		void SetOrthographicNearClip(float nearClip) { m_OrthographicNearClip = nearClip; CalculateProjection(); }
		float GetOrthographicFarClip() const { return m_OrthographicFarClip; }
		void SetOrthographicFarClip(float farClip) { m_OrthographicFarClip = farClip; CalculateProjection(); }

		ProjectionType GetProjectionType() const { return m_ProjectionType; }
		void SetProjectionType(ProjectionType type) { m_ProjectionType = type; CalculateProjection(); }

	private:
		glm::mat4 m_Projection = glm::mat4(1.0f);
		ProjectionType m_ProjectionType = ProjectionType::Perspective;
		float m_AspectRatio = 1280.0f / 720.0f;

		float m_PerspectiveFOV = glm::radians(45.0f);
		float m_PerspectiveNearClip = 0.01f;
		float m_PerspectiveFarClip = 1000.0f;

		float m_OrthographicZoom = 1.0f;
		float m_OrthographicNearClip = -1.0f;
		float m_OrthographicFarClip = 1.0f;

		void CalculateProjection();
	};
}