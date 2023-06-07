#pragma once

#include "OrthographicCamera.h"
#include "FoolsEngine\Events\Event.h"

namespace fe
{
	class OrthographicCameraController
	{
	public:
		OrthographicCameraController(float aspectRatio, bool rotation = false);

		void OnUpdate();
		void OnEvent(Ref<Events::Event> event);

		float GetZoomLevel() const { return m_ZoomLevel; }
		float SetZoomLevel(float lvl) { m_ZoomLevel = lvl; }
		void Resize(float width, float hight);
		
		OrthographicCamera& GetCamera() { return m_Camera; }
		const OrthographicCamera& GetCamera() const { return m_Camera; }
	private:
		float m_AspectRatio;
		float m_ZoomLevel = 1.0f;
		OrthographicCamera m_Camera;

		bool m_Rotation;

		glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
		//in degrees
		float m_CameraRotation = 0.0f;
		float m_CameraTranslationSpeed = 5.0f;
		float m_CameraRotationSpeed = 20.0f;

		void OnMouseScrolled(Ref<Events::MouseScrolledEvent> event);
		void OnWindowResized(Ref<Events::WindowResizeEvent> event);
	};
}