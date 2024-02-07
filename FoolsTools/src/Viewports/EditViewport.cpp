#include "EditViewport.h"

namespace fe
{
	EditViewport::EditViewport()
	{
		FramebufferData::SpecificationBuilder specBuilder;
		specBuilder
			.SetWidth(1280)
			.SetHight(720)
			.SetDepthStencilAttachmentDataFormat(TextureData::Format::DEPTH24STENCIL8)
			.SetColorAttachmentSpecifications({
				{ "Main Framebuffer", TextureData::Components::RGBA, TextureData::Format::RGBA8}
				});
		m_Framebuffer = Framebuffer::Create(specBuilder.Create());

		m_CameraController = CreateScope<EditorCameraController>(1280.0f, 720.0f);
	}

	void EditViewport::UpdateCamera()
	{
		if (Application::Get().GetImguiLayer()->IsBlocking() || !m_IsVisible)
			return;
		m_CameraController->OnUpdate();
	}

	void EditViewport::RenderScene()
	{
		if (!m_IsVisible)
			return;

		m_Framebuffer->Bind();
		Renderer2D::RenderScene(*m_Scene, m_CameraController->GetCamera(), m_CameraController->GetTransform());
		m_Framebuffer->Unbind();
	}

	void EditViewport::OnEvent(Ref<Events::Event> event)
	{
		Events::EventDispacher dispacher(event);
		dispacher.Dispach<Events::KeyPressedEvent>(FE_BIND_EVENT_HANDLER(EditViewport::OnKeyPressedEvent));

		if (event->Handled || event->Owned)
			return;

		m_CameraController->OnEvent(event);
	}

	void EditViewport::OnImGuiRender()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		m_IsVisible = ImGui::Begin("Edit Scene");
		ImGui::PopStyleVar();

		m_VieportFocus = ImGui::IsWindowFocused();
		m_VieportHover = ImGui::IsWindowHovered();

		Application::Get().GetImguiLayer()->BlockEvents(!(m_VieportFocus || m_VieportHover) && m_IsVisible);

		auto vidgetSize = ImGui::GetContentRegionAvail();
		glm::vec2 newViewPortSize = { vidgetSize.x, vidgetSize.y }; // most likely simple cast possible, but still different data types from different librarys

		if (m_ViewportSize != newViewPortSize)
		{
			// there is a bug in ImGui that is causing GetContentRegionAvail() to report wrong values in first frame
			// this is a workaround that prevents creation of framebuffer with 0 hight or with
			if (newViewPortSize.x == 0 || newViewPortSize.y == 0)
				newViewPortSize = { 1, 1 };

			m_Framebuffer->Resize((uint32_t)newViewPortSize.x, (uint32_t)newViewPortSize.y);
			m_ViewportSize = newViewPortSize;
			m_CameraController->Resize(newViewPortSize.x, newViewPortSize.y);
		}

		auto fbID = m_Framebuffer->GetColorAttachmentID();
		ImGui::Image((void*)(uint64_t)fbID, vidgetSize, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		//Gizmos

		Entity selectedEntity(m_SelectedEntityID, m_Scene->GetGameplayWorld());

		if (selectedEntity)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, newViewPortSize.x, newViewPortSize.y);

			// Camera
			auto cameraEntity = m_Scene->GetGameplayWorld()->GetEntityWithPrimaryCamera();
			const auto& camera = m_CameraController->GetCamera();
			const glm::mat4& cameraProjection = camera.GetProjectionMatrix();
			glm::mat4 cameraView = glm::inverse(m_CameraController->GetTransform().GetTransform());

			// Entity transform
			auto& tc = selectedEntity.GetTransformHandle().GetGlobal();
			glm::mat4 transformMatrix = tc.GetTransform();

			// Snapping
			bool snap = InputPolling::IsKeyPressed(InputCodes::LeftControl);
			float snapValue = 0.5f; // Snap to 0.5m for translation/scale
			// Snap to 45 degrees for rotation
			if (m_GuizmoType == ImGuizmo::OPERATION::ROTATE)
				snapValue = 45.0f;

			float snapValues[3] = { snapValue, snapValue, snapValue };

			ImGuizmo::Manipulate(
				glm::value_ptr(cameraView),
				glm::value_ptr(cameraProjection),
				(ImGuizmo::OPERATION)m_GuizmoType,
				ImGuizmo::LOCAL,
				glm::value_ptr(transformMatrix),
				nullptr,
				snap ? snapValues : nullptr
			);

			if (ImGuizmo::IsUsing())
			{
				auto mosePos = ImGui::GetMousePos();
				Transform transform;
				Math::DecomposeTransform(transformMatrix, transform);
				transform.Rotation = glm::degrees(transform.Rotation);
				selectedEntity.GetTransformHandle().SetGlobal(transform);
			}
		}

		ImGui::End();
	}

	void EditViewport::OnKeyPressedEvent(Ref<Events::KeyPressedEvent> event)
	{
		if (event->GetRepeatCount() == 0)
		{
			bool control = InputPolling::IsKeyPressed(InputCodes::LeftControl) || InputPolling::IsKeyPressed(InputCodes::RightControl);
			bool shift = InputPolling::IsKeyPressed(InputCodes::LeftShift) || InputPolling::IsKeyPressed(InputCodes::RightShift);

			switch (event->GetKeyCode())
			{
			case InputCodes::Z:
				m_GuizmoType = ImGuizmo::OPERATION::TRANSLATE;
				event->Handle();
				return;
			case InputCodes::X:
				m_GuizmoType = ImGuizmo::OPERATION::ROTATE;
				event->Handle();
				return;
			case InputCodes::C:
				m_GuizmoType = ImGuizmo::OPERATION::SCALE;
				event->Handle();
				return;
			case InputCodes::V:
				m_GuizmoType = ImGuizmo::OPERATION::UNIVERSAL;
				event->Handle();
				return;
			}
		}
	}
}