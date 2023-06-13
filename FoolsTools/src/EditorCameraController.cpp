#include "FE_pch.h"

#include <FoolsEngine.h>

#include "EditorCameraController.h"

namespace fe
{
	EditorCameraController::EditorCameraController(float aspectRatio)
		: m_Camera(CCamera::Orthographic, aspectRatio) { }

	void EditorCameraController::OnUpdate()
	{
		FE_PROFILER_FUNC();

		auto dt = Time::DeltaTime();

		glm::vec3& position = m_Transform.Position;
		float& rotation = m_Transform.Rotation.z;
		float rotationInRadians = glm::radians(rotation);
		
		int horizontalDir = InputPolling::IsKeyPressed(InputCodes::D) - InputPolling::IsKeyPressed(InputCodes::A);
		float distance = horizontalDir * m_TranslationSpeed * m_Camera.Zoom * dt;

		position.x += distance * cos(rotationInRadians);
		position.y += distance * sin(rotationInRadians);
			
		int verticalDir = InputPolling::IsKeyPressed(InputCodes::W) - InputPolling::IsKeyPressed(InputCodes::S);
		distance = verticalDir * m_TranslationSpeed * m_Camera.Zoom * dt;

		position.x += distance * -sin(rotationInRadians);
		position.y += distance * cos(rotationInRadians);

		float angle = (InputPolling::IsKeyPressed(InputCodes::E) - InputPolling::IsKeyPressed(InputCodes::Q)) * m_RotationSpeed * dt;
		rotation += angle;

		if (rotation > 180.0f)
			rotation -= 360.0f;
		else if (rotation < -180.0f)
			rotation += 360.0f;

		if (!angle && -3.0f < rotation && rotation < 3.0f)
			rotation = 0.0f;
	}

	void EditorCameraController::OnEvent(Ref<Events::Event> event)
	{
		Events::EventDispacher dispacher(event);
		dispacher.Dispach<Events::MouseScrolledEvent>(FE_BIND_EVENT_HANDLER(EditorCameraController::OnMouseScrolled));
	}

	void EditorCameraController::Resize(float width, float hight)
	{
		m_Camera.SetViewportSize((uint32_t)width, (uint32_t)hight);
	}

	void EditorCameraController::OnMouseScrolled(Ref<Events::MouseScrolledEvent> event)
	{
		m_Camera.Zoom -= event->GetOffsetY() * 0.25f;
		m_Camera.Zoom = std::max(m_Camera.Zoom, 0.25f);

		event->Handled = true;
	}
}