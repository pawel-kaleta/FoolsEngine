#include "FE_pch.h"
#include "OrtographicCamera.h"
#include <glm\gtc\matrix_transform.hpp>

#include "Renderer.h"

namespace fe
{
	OrtographicCamera::OrtographicCamera(float left, float right, float bottom, float top)
		: m_ViewMatrix(1.0f), m_VMatrixInvalid(false)
	{
		FE_PROFILER_FUNC();
		m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		RecalculateViewMatrix();
	}

	void fe::OrtographicCamera::RecalculateViewMatrix()
	{
		FE_PROFILER_FUNC();
		glm::mat4 transformationMatrix =
			glm::translate(glm::mat4(1.0f), m_Position)
			*
			glm::rotate(glm::mat4(1.0f), m_Rotation, glm::vec3(0,0,1));

		m_ViewMatrix = glm::inverse(transformationMatrix);

		switch (Renderer::GetNativeAPI())
		{
		case RendererAPI::NativeAPI::OpenGL:
			m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
			break;
		default:
			FE_CORE_ASSERT(false, "Unkown RendererAPI!");
			return;
		}

		m_VMatrixInvalid = false;
	}

}