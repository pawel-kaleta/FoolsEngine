#include "FE_pch.h"
#include "OrthographicCamera.h"
#include <glm\gtc\matrix_transform.hpp>

#include "Renderer.h"

namespace fe
{
	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
		: m_ViewMatrix(1.0f), m_VMatrixInvalid(false)
	{
		FE_PROFILER_FUNC();
		m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		RecalculateViewMatrix();
	}

	void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
	{
		m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		m_VMatrixInvalid = true;
	}

	void fe::OrthographicCamera::RecalculateViewMatrix()
	{
		FE_PROFILER_FUNC();
		glm::mat4 transformationMatrix =
			glm::translate(glm::mat4(1.0f), m_Position)
			*
			glm::rotate(glm::mat4(1.0f), m_Rotation, glm::vec3(0,0,1));

		m_ViewMatrix = glm::inverse(transformationMatrix);

		switch (Renderer::GetGDItype())
		{
		case GDIType::OpenGL:
			m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
			break;
		default:
			FE_CORE_ASSERT(false, "Unkown GDI!");
			return;
		}

		m_VMatrixInvalid = false;
	}

}