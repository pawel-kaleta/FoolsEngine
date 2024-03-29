#include "FE_pch.h"
#include "Camera.h"

#include <glm\gtc\matrix_transform.hpp>

namespace fe
{
	void Camera::SetOrthographic(float zoom, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Orthographic;
		m_OrthographicZoom = zoom;
		m_OrthographicNearClip = nearClip;
		m_OrthographicFarClip = farClip;
		CalculateProjection();
	}

	void Camera::SetPerspective(float verticalFOV, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Perspective;
		m_PerspectiveFOV = verticalFOV;
		m_PerspectiveNearClip = nearClip;
		m_PerspectiveFarClip = farClip;
		CalculateProjection();
	}

	void Camera::CalculateProjection()
	{
		if (m_ProjectionType == ProjectionType::Perspective)
		{
			m_Projection = glm::perspective(m_PerspectiveFOV, m_AspectRatio, m_PerspectiveNearClip, m_PerspectiveFarClip);
		}
		else
		{
			float top = m_OrthographicZoom * 0.5f;
			float bottom = m_OrthographicZoom * -0.5f;
			float right = m_AspectRatio * top;
			float left = m_AspectRatio * bottom;

			m_Projection = glm::ortho(left, right, bottom, top, m_OrthographicNearClip, m_OrthographicFarClip);
		}
	}
}