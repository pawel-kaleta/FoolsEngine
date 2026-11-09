#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace fe::Render::Representation
{
	void Camera::SetOrthographic(float zoom, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Orthographic;
		m_OrthographicData.m_Zoom = zoom;
		m_OrthographicData.m_NearClip = nearClip;
		m_OrthographicData.m_FarClip = farClip;
		CalculateProjectionOrthographic();
	}

	void Camera::SetPerspective(float verticalFOV, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Perspective;
		m_PerspectiveData.m_FOV = verticalFOV;
		m_PerspectiveData.m_NearClip = nearClip;
		m_PerspectiveData.m_FarClip = farClip;
		CalculateProjectionPerspective();
	}

	void Camera::CalculateProjection()
	{
		if (m_ProjectionType == ProjectionType::Perspective)
			CalculateProjectionPerspective();
		else
			CalculateProjectionOrthographic();
	}

	glm::mat4 MakeInfReversedZProjRH(float fovY_radians, float aspectWbyH, float zNear)
	{
		float f = 1.0f / tan(fovY_radians / 2.0f);
		return glm::mat4(
			f / aspectWbyH,  0.0f,  0.0f,  0.0f,
			          0.0f,	    f,  0.0f,  0.0f,
			          0.0f,  0.0f,  0.0f, -1.0f,
			          0.0f,  0.0f, zNear,  0.0f);
	}

	void Camera::CalculateProjectionPerspective()
	{
		FE_CORE_ASSERT(m_ProjectionType == ProjectionType::Perspective, "Wrong camera ProjectionType");

		auto& data = m_PerspectiveData;
		//m_Projection = glm::perspective(data.m_FOV, m_AspectRatio, data.m_NearClip, data.m_FarClip);
		//m_Projection = glm::tweakedInfinitePerspective(data.m_FOV, m_AspectRatio, data.m_NearClip); // reverse z
		m_Projection = MakeInfReversedZProjRH(data.m_FOV, m_AspectRatio, data.m_NearClip);
	}

	void Camera::CalculateProjectionOrthographic()
	{
		FE_CORE_ASSERT(m_ProjectionType == ProjectionType::Orthographic, "Wrong camera ProjectionType");

		auto& data = m_OrthographicData;
		float top = data.m_Zoom * 0.5f;
		float bottom = data.m_Zoom * -0.5f;
		float right = m_AspectRatio * top;
		float left = m_AspectRatio * bottom;

		m_Projection = glm::ortho(left, right, bottom, top, data.m_NearClip, data.m_FarClip);
	}
}