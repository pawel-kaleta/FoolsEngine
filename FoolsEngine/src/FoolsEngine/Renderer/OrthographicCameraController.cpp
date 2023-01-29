#include "FE_pch.h"

#include "OrthographicCameraController.h"
#include "FoolsEngine\Core\InputPolling.h"
#include "FoolsEngine\Core\Application.h"

namespace fe
{
	OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation)
		: m_AspectRatio(aspectRatio), m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel), m_Rotation(rotation) { }

	void OrthographicCameraController::OnUpdate()
	{
		auto dt = Time::DeltaTime();

		m_CameraPosition.x += (InputPolling::IsKeyPressed(InputCodes::D) - InputPolling::IsKeyPressed(InputCodes::A)) * m_CameraTranslationSpeed * dt;
		m_CameraPosition.y += (InputPolling::IsKeyPressed(InputCodes::W) - InputPolling::IsKeyPressed(InputCodes::S)) * m_CameraTranslationSpeed * dt;

		m_Camera.SetPosition(m_CameraPosition);

		if (m_Rotation)
		{
			m_CameraRotation += (InputPolling::IsKeyPressed(InputCodes::E) - InputPolling::IsKeyPressed(InputCodes::Q)) * m_CameraRotationSpeed * dt;

			m_Camera.SetRotation(m_CameraRotation);
		}

		m_CameraTranslationSpeed = m_ZoomLevel;
	}

	void OrthographicCameraController::OnEvent(Ref<Events::Event> event)
	{
		Events::EventDispacher dispacher(event);
		dispacher.Dispach<Events::MouseScrolledEvent>(FE_BIND_EVENT_HANDLER(OrthographicCameraController::OnMouseScrolled));
		dispacher.Dispach<Events::WindowResizeEvent>(FE_BIND_EVENT_HANDLER(OrthographicCameraController::OnWindowResized));
	}

	void OrthographicCameraController::OnMouseScrolled(Ref<Events::MouseScrolledEvent> event)
	{
		m_ZoomLevel -= event->GetOffsetY() * 0.25f;
		m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);

		event->Handled = true;
	}

	void OrthographicCameraController::OnWindowResized(Ref<Events::WindowResizeEvent> event)
	{
		m_AspectRatio = (float)event->GetWidth() / (float)event->GetHeight();
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);

		event->Handled = true;
	}
}