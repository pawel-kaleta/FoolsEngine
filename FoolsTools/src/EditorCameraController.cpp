#include "FE_pch.h"

#include <FoolsEngine.h>

#include "EditorCameraController.h"

namespace fe
{
	EditorCameraController::EditorCameraController(float width, float hight)
		: m_ViewportSize({width, hight})
	{
		m_Camera.SetViewportSize((uint32_t)width, (uint32_t)hight);
		m_Transform.Position.z = 2.0f;
	}

	void EditorCameraController::OnUpdate()
	{
		FE_PROFILER_FUNC();

		if (Application::Get().GetImguiLayer()->IsBlocking())
			return;

		int inputAxisDA = InputPolling::IsKeyPressed(InputCodes::D) - InputPolling::IsKeyPressed(InputCodes::A);
		int inputAxisWS = InputPolling::IsKeyPressed(InputCodes::W) - InputPolling::IsKeyPressed(InputCodes::S);
		int inputAxisEQ = InputPolling::IsKeyPressed(InputCodes::E) - InputPolling::IsKeyPressed(InputCodes::Q);
		bool ctrl = InputPolling::IsKeyPressed(InputCodes::LeftControl);

		if (!(inputAxisDA || inputAxisWS || inputAxisEQ))
			return;

		if (ctrl)
			Rotate(inputAxisDA, inputAxisWS, inputAxisEQ);
		else
		{
			if (m_Camera.GetProjectionType() == Camera::ProjectionType::Perspective)
				Move(inputAxisDA, inputAxisEQ, inputAxisWS);
			else
				Move(inputAxisDA, inputAxisWS, inputAxisEQ);
		}
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
		auto delta = event->GetOffsetY();

		Zoom(delta);

		event->Handled = true;
	}

	void EditorCameraController::Rotate(int headingDir, int pitchDir, int bankDir)
	{
		auto angleDelta = Time::DeltaTime() * m_RotationSpeed;

		float headingDelta = -headingDir * angleDelta;
		float pitchDelta = pitchDir * angleDelta;
		float bankDelta = bankDir * angleDelta;

		auto orientationHeadingDelta = glm::angleAxis(glm::radians(headingDelta), GetDirectionUp());
		auto orientationPitchDelta = glm::angleAxis(glm::radians(pitchDelta), GetDirectionRight());
		auto orientationBankDelta = glm::angleAxis(glm::radians(bankDelta), GetDirectionForward());

		auto newOrientation = orientationBankDelta * orientationPitchDelta * orientationHeadingDelta * GetOrientation();

		m_Transform.Rotation = glm::degrees(glm::eulerAngles(newOrientation));
	}

	void EditorCameraController::Move(int horizontalDir, int verticalDir, int viewDir)
	{
		auto step = Time::DeltaTime() * m_MoveSpeed;

		float horizontalStep = horizontalDir * step;
		float verticalStep = verticalDir * step;
		float viewStep = viewDir * step;

		m_Transform.Position += verticalStep * GetDirectionUp();
		m_Transform.Position += horizontalStep * GetDirectionRight();
		m_Transform.Position += viewStep * GetDirectionForward();
	}

	void EditorCameraController::Zoom(float delta)
	{
		if (m_Camera.GetProjectionType() == Camera::ProjectionType::Perspective)
		{
			auto FOV = m_Camera.GetPerspectiveFOV();
			FOV -= delta * 0.05f;
			FOV = std::clamp(FOV, glm::radians(10.0f), glm::radians(160.0f));
			m_Camera.SetPerspectiveFOV(FOV);
		}
		else
		{
			auto zoom = m_Camera.GetOrthographicZoom();
			zoom -= delta * 0.25f;
			zoom = std::max(zoom, 0.25f);
			m_Camera.SetOrthographicZoom(zoom);
		}
	}

	glm::vec3 EditorCameraController::GetDirectionUp() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::vec3 EditorCameraController::GetDirectionRight() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	glm::vec3 EditorCameraController::GetDirectionForward() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}

	glm::quat EditorCameraController::GetOrientation() const
	{
		return glm::quat(glm::radians(m_Transform.Rotation));
	}

	void EditorCameraController::RenderWidget()
	{
		ImGui::PushItemWidth(-ImGui::GetContentRegionAvail().x * 0.5f);

		auto& transform = GetTransform();

		ImGui::DragFloat3("Position", glm::value_ptr(transform.Position), 0.01f);
		ImGui::DragFloat3("Rotation", glm::value_ptr(transform.Rotation), 0.10f);
		ImGui::DragFloat3("Scale"   , glm::value_ptr(transform.Scale   ), 0.01f);

		constexpr char* projectionTypeStrings[] = { "Orthographic", "Perspective" };
		const char* currentProjectionTypeString = projectionTypeStrings[(int)m_Camera.GetProjectionType()];

		if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
		{
			for (int i = 0; i < 2; i++)
			{
				bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
				if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
				{
					currentProjectionTypeString = projectionTypeStrings[i];
					m_Camera.SetProjectionType((Camera::ProjectionType)i);
				}

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		if (m_Camera.GetProjectionType() == Camera::ProjectionType::Perspective)
		{
			float verticalFov = glm::degrees(m_Camera.GetPerspectiveFOV());
			if (ImGui::DragFloat("Field of View", &verticalFov))
				m_Camera.SetPerspectiveFOV(glm::radians(verticalFov));

			float orthoNear = m_Camera.GetPerspectiveNearClip();
			if (ImGui::DragFloat("Near Clip", &orthoNear))
				m_Camera.SetPerspectiveNearClip(orthoNear);

			float orthoFar = m_Camera.GetPerspectiveFarClip();
			if (ImGui::DragFloat("Far Clip", &orthoFar))
				m_Camera.SetPerspectiveFarClip(orthoFar);
		}
		else
		{
			float zoom = m_Camera.GetOrthographicZoom();
			if (ImGui::DragFloat("Zoom", &zoom))
				m_Camera.SetOrthographicZoom(zoom);

			float orthoNear = m_Camera.GetOrthographicNearClip();
			if (ImGui::DragFloat("Near Clip", &orthoNear))
				m_Camera.SetOrthographicNearClip(orthoNear);

			float orthoFar = m_Camera.GetOrthographicFarClip();
			if (ImGui::DragFloat("Far Clip", &orthoFar))
				m_Camera.SetOrthographicFarClip(orthoFar);
		}
	}
}