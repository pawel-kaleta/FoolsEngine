#include "FE_pch.h"

#include <FoolsEngine.h>

#include "EditorCameraController.h"

namespace fe
{
	EditorCameraController::EditorCameraController(float width, float hight)
		: m_ViewportSize({width, hight})
	{
		FE_PROFILER_FUNC();

		m_Camera.SetViewportSize((uint32_t)width, (uint32_t)hight);
		m_Transform.Shift.z = 2.0f;
	}

	void EditorCameraController::OnUpdate()
	{
		FE_PROFILER_FUNC();

		if (Application::Get().m_ImGuiLayer->BlockEvents)
			return;

		int input_axis_DA = InputPolling::IsKeyPressed(InputCodes::D) - InputPolling::IsKeyPressed(InputCodes::A);
		int input_axis_WS = InputPolling::IsKeyPressed(InputCodes::W) - InputPolling::IsKeyPressed(InputCodes::S);
		int input_axis_EQ = InputPolling::IsKeyPressed(InputCodes::E) - InputPolling::IsKeyPressed(InputCodes::Q);
		bool ctrl = InputPolling::IsKeyPressed(InputCodes::LeftControl);

		if (!(input_axis_DA || input_axis_WS || input_axis_EQ))
			return;

		if (ctrl)
			Rotate(input_axis_DA, input_axis_WS, input_axis_EQ);
		else
		{
			if (m_Camera.m_ProjectionType == Camera::ProjectionType::Perspective)
				Move(input_axis_DA, input_axis_EQ, input_axis_WS);
			else
				Move(input_axis_DA, input_axis_WS, input_axis_EQ);
		}
	}

	void EditorCameraController::OnEvent(Ref<Events::Event> event)
	{
		Events::EventDispacher dispacher(event);
		dispacher.Dispach<Events::MouseScrolledEvent>(FE_BIND_EVENT_HANDLER(EditorCameraController::OnMouseScrolled));
	}

	void EditorCameraController::Resize(float width, float hight)
	{
		FE_PROFILER_FUNC();

		m_Camera.SetViewportSize((uint32_t)width, (uint32_t)hight);
	}

	void EditorCameraController::OnMouseScrolled(Ref<Events::MouseScrolledEvent> event)
	{
		auto delta = event->GetOffsetY();

		Zoom(delta);

		event->Handle();
	}

	void EditorCameraController::Rotate(int headinGAPIr, int pitchDir, int bankDir)
	{
		FE_PROFILER_FUNC();

		auto angle_delta = Time::DeltaTime() * m_RotationSpeed;

		float heading_delta = -headinGAPIr * angle_delta;
		float pitch_delta = pitchDir * angle_delta;
		float bank_delta = bankDir * angle_delta;

		auto orientation_heading_delta = glm::angleAxis(glm::radians(heading_delta), GetDirectionUp());
		auto orientation_pitch_delta = glm::angleAxis(glm::radians(pitch_delta), GetDirectionRight());
		auto orientation_bank_delta = glm::angleAxis(glm::radians(bank_delta), GetDirectionForward());

		auto new_orientation = orientation_bank_delta * orientation_pitch_delta * orientation_heading_delta * GetOrientation();

		m_Transform.Rotation = glm::degrees(glm::eulerAngles(new_orientation));
	}

	void EditorCameraController::Move(int horizontalDir, int verticalDir, int viewDir)
	{
		FE_PROFILER_FUNC();

		auto step = Time::DeltaTime() * m_MoveSpeed;

		float horizontal_step = horizontalDir * step;
		float vertical_step = verticalDir * step;
		float view_step = viewDir * step;

		m_Transform.Shift += vertical_step * GetDirectionUp();
		m_Transform.Shift += horizontal_step * GetDirectionRight();
		m_Transform.Shift += view_step * GetDirectionForward();
	}

	void EditorCameraController::Zoom(float delta)
	{
		FE_PROFILER_FUNC();

		if (m_Camera.m_ProjectionType == Camera::ProjectionType::Perspective)
		{
			auto FOV = m_Camera.m_PerspectiveData.m_FOV;
			FOV -= delta * 0.05f;
			FOV = std::clamp(FOV, glm::radians(10.0f), glm::radians(160.0f));
			m_Camera.SetPerspectiveFOV(FOV);
		}
		else
		{
			auto zoom = m_Camera.m_OrthographicData.m_Zoom;
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
		FE_PROFILER_FUNC();

		ImGui::PushItemWidth(-ImGui::GetContentRegionAvail().x * 0.5f);

		auto& transform = GetTransform();

		ImGui::DragFloat3("Shift", glm::value_ptr(transform.Shift), 0.01f);
		ImGui::DragFloat3("Rotation", glm::value_ptr(transform.Rotation), 0.10f);
		ImGui::DragFloat3("Scale"   , glm::value_ptr(transform.Scale   ), 0.01f);

		constexpr const char* projection_type_strings[] = { "Orthographic", "Perspective" };
		const char* current_projection_type_string = projection_type_strings[m_Camera.m_ProjectionType.ToInt()];

		if (ImGui::BeginCombo("Projection", current_projection_type_string))
		{
			for (int i = 0; i < 2; i++)
			{
				bool is_selected = current_projection_type_string == projection_type_strings[i];
				if (ImGui::Selectable(projection_type_strings[i], is_selected))
				{
					current_projection_type_string = projection_type_strings[i];
					Camera::ProjectionType projection;
					projection.FromInt(i);
					m_Camera.SetProjectionType(projection);
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		if (m_Camera.m_ProjectionType == Camera::ProjectionType::Perspective)
		{
			const auto& data = m_Camera.m_PerspectiveData;
			float fov = glm::degrees(data.m_FOV);
			if (ImGui::DragFloat("Field of View", &fov))
				m_Camera.SetPerspectiveFOV(glm::radians(fov));

			float near_clip = data.m_NearClip;
			if (ImGui::DragFloat("Near Clip", &near_clip))
				m_Camera.SetPerspectiveNearClip(near_clip);

			float far_clip = data.m_NearClip;
			if (ImGui::DragFloat("Far Clip", &far_clip))
				m_Camera.SetPerspectiveFarClip(far_clip);
		}
		else
		{
			const auto& data = m_Camera.m_OrthographicData;

			float zoom = data.m_Zoom;
			if (ImGui::DragFloat("Zoom", &zoom))
				m_Camera.SetOrthographicZoom(zoom);

			float near_clip = data.m_NearClip;
			if (ImGui::DragFloat("Near Clip", &near_clip))
				m_Camera.SetOrthographicNearClip(near_clip);

			float far_clip = data.m_FarClip;
			if (ImGui::DragFloat("Far Clip", &far_clip))
				m_Camera.SetOrthographicFarClip(far_clip);
		}
	}
}