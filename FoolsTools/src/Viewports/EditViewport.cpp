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
				{ "Final frame", TextureData::Components::RGBA_F, TextureData::Format::RGBA_FLOAT_8 },
				{ "EntityID"   , TextureData::Components::R_UI  , TextureData::Format::R_UINT_32    }
			});
		m_Framebuffer = Framebuffer::Create(specBuilder.Create());

		m_CameraController = CreateScope<EditorCameraController>(1280.0f, 720.0f);
	}

	void EditViewport::OnUpdate()
	{
		if (Application::Get().GetImguiLayer()->IsBlocking() || !m_IsVisible)
			return;
		m_CameraController->OnUpdate();

		auto [mx, my] = ImGui::GetMousePos();
		mx -= m_ViewportBounds[0].x;
		my -= m_ViewportBounds[0].y;
		glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
		my = viewportSize.y - my;
		int mouseX = (int)mx;
		int mouseY = (int)my;

		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
		{
			int attachmentIndex = m_Framebuffer->GetColorAttachmentIndex("EntityID");
			m_Framebuffer->Bind();
			m_Framebuffer->ReadPixel(attachmentIndex, mouseX, mouseY, &m_HoveredEntityID);
			m_Framebuffer->Unbind();
		}
		else
		{
			m_HoveredEntityID = NullEntityID;
		}
	}

	void EditViewport::RenderScene()
	{
		if (!m_IsVisible)
			return;


		Renderer2D::RenderScene(*m_Scene, m_CameraController->GetCamera(), m_CameraController->GetTransform(), *m_Framebuffer.get());
	}

	void EditViewport::OnEvent(Ref<Events::Event> event)
	{
		if (Application::Get().GetImguiLayer()->IsBlocking() || !m_IsVisible)
			return;

		Events::EventDispacher dispacher(event);
		dispacher.Dispach<Events::KeyPressedEvent>(FE_BIND_EVENT_HANDLER(EditViewport::OnKeyPressedEvent));

		if (event->Handled || event->Owned)
			return;

		dispacher.Dispach<Events::MouseButtonPressedEvent>(FE_BIND_EVENT_HANDLER(EditViewport::OnMouseButtonPressedEvent));

		if (event->Handled || event->Owned)
			return;

		m_CameraController->OnEvent(event);
	}

	void EditViewport::OnImGuiRender()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		m_IsVisible = ImGui::Begin("Edit Scene");
		ImGui::PopStyleVar();

		m_EntityIDSelectionRequest = NullEntityID;
		if (m_EntityClicked)
			m_EntityIDSelectionRequest = m_HoveredEntityID;
		m_EntityClicked = false;

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

		RenderGuizmos();

		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();
		m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		ImGui::End();
	}

	void EditViewport::OnKeyPressedEvent(Ref<Events::KeyPressedEvent> event)
	{
		if (event->GetRepeatCount() == 0 && !ImGuizmo::IsUsing())
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

	void EditViewport::OnMouseButtonPressedEvent(Ref<Events::MouseButtonPressedEvent> event)
	{
		if (event->GetMouseButtonCode() == InputCodes::MouseButtonLeft)
		{
			if (!ImGuizmo::IsOver() && !ImGuizmo::IsUsingAny())
			{
				m_EntityClicked = true;
			}
		}
	}

	void EditViewport::RenderGuizmos()
	{
		Entity selectedEntity(m_SelectedEntityID, m_Scene->GetGameplayWorld());

		if (!selectedEntity)
			return;

		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();

		ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);

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
}