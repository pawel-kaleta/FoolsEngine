#pragma once

#include "FoolsEngine/Foundation/Utils/DeclareEnum.h"

#include <glm/glm.hpp>

namespace fe
{
	class Camera
	{
	public:
		FE_DECLARE_ENUM(ProjectionType, Orthographic, Perspective);

		Camera() { CalculateProjectionPerspective(); }
		~Camera() = default;

		glm::mat4 GetProjectionMatrix() const { return m_Projection; };
		operator glm::mat4() { return m_Projection; }
		operator const glm::mat4() const { return m_Projection; }

		void SetOrthographic(float zoom, float nearClip, float farClip);
		void SetPerspective(float verticalFOV, float nearClip, float farClip);

		void SetViewportSize(uint32_t width, uint32_t hight) { m_AspectRatio = (float)width / (float)hight; CalculateProjection(); }

		//radians
		void SetPerspectiveFOV(float FOV) { m_PerspectiveData.m_FOV = FOV; CalculateProjectionPerspective(); }
		void SetPerspectiveNearClip(float nearClip) { m_PerspectiveData.m_NearClip = nearClip; CalculateProjectionPerspective(); }
		void SetPerspectiveFarClip(float farClip) { m_PerspectiveData.m_FarClip = farClip; CalculateProjectionPerspective(); }

		void SetOrthographicZoom(float zoom) { m_OrthographicData.m_Zoom = zoom; CalculateProjectionOrthographic(); }
		void SetOrthographicNearClip(float nearClip) { m_OrthographicData.m_NearClip = nearClip; CalculateProjectionOrthographic(); }
		void SetOrthographicFarClip(float farClip) { m_OrthographicData.m_FarClip = farClip; CalculateProjectionOrthographic(); }

		void SetProjectionType(ProjectionType type) { m_ProjectionType = type; CalculateProjection(); }

		ProjectionType m_ProjectionType = ProjectionType::Perspective;

		glm::mat4 m_Projection = glm::mat4(1.0f);
		float m_AspectRatio = 1280.0f / 720.0f;

		struct
		{
			//radians
			float m_FOV = glm::radians(60.0f);
			float m_NearClip = 0.01f;
			float m_FarClip = 1000.0f;
		} m_PerspectiveData;

		struct
		{
			float m_Zoom = 1.0f;
			float m_NearClip = -1.0f;
			float m_FarClip = 1.0f;
		} m_OrthographicData;
		

	private:
		void CalculateProjection();
		void CalculateProjectionPerspective();
		void CalculateProjectionOrthographic();
	};
}