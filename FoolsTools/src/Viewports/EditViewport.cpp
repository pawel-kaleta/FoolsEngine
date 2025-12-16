#include "EditViewport.h"

namespace fe
{
	EditViewport::EditViewport()
	{
		FE_PROFILER_FUNC();

		//Description::Framebuffer::SpecificationBuilder spec_builder;
		//spec_builder
		//	.SetWidth(1280)
		//	.SetHight(720)
		//	.SetDepthStencilAttachmentFormat(Description::Texture::Format::DEPTH24STENCIL8)
		//	.AddColorAttachmentSpecification(Description::Framebuffer::Attachment("Final Frame", Description::Texture::Format::RGBA_8))
		//	.AddColorAttachmentSpecification(Description::Framebuffer::Attachment("EntityID", Description::Texture::Format::R_UINT_32));
		m_Framebuffer.reset(new Resource::Framebuffer_OpenGL());

		auto& lib = Description::Library::Get();
		m_Framebuffer->SpecificationID = lib.FramebufferSpecs.size();
		auto& framebuffer_spec = lib.FramebufferSpecs.emplace_back();
		framebuffer_spec.Width = 1280;
		framebuffer_spec.Height = 720;
		framebuffer_spec.DepthStencilFormat = Description::Texture::Format::DEPTH24STENCIL8;
		framebuffer_spec.ColorAttachments.emplace_back("Final Frame", Description::Texture::Format::RGBA_8);
		framebuffer_spec.ColorAttachments.emplace_back("EntityID", Description::Texture::Format::R_UINT_32);
		framebuffer_spec.Type = Description::Texture::Type::Texture2D;

		m_Framebuffer->Create();

		m_CameraController = CreateScope<EditorCameraController>(1280.0f, 720.0f);
	}

	void EditViewport::OnUpdate()
	{
		FE_PROFILER_FUNC();

		if (Application::Get().m_ImGuiLayer->BlockEvents || !m_IsVisible)
			return;
		m_CameraController->OnUpdate();
	}

	void EditViewport::RenderScene()
	{
		FE_PROFILER_FUNC();

		if (!m_IsVisible)
			return;

		auto scene_observer = m_Scene.Observe();
		Renderer::RenderScene(scene_observer, m_CameraController->GetCamera(), m_CameraController->GetTransform(), *m_Framebuffer.get());
	}

	void EditViewport::OnEvent(Ref<Events::Event> event)
	{
		if (Application::Get().m_ImGuiLayer->BlockEvents || !m_IsVisible)
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
		FE_PROFILER_FUNC();

		ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		m_IsVisible = ImGui::Begin("Edit Scene");
		ImGui::PopStyleVar();

		m_EntityIDSelectionRequest = NullEntityID;
		if (m_EntityClicked)
		{
			m_EntityIDSelectionRequest = ReadEntityIDfromBuffer();
		}
		m_EntityClicked = false;

		m_VieportFocus = ImGui::IsWindowFocused();
		m_VieportHover = ImGui::IsWindowHovered();

		Application::Get().m_ImGuiLayer->BlockEvents = m_IsVisible && !(m_VieportFocus || m_VieportHover);

		auto vidget_size = ImGui::GetContentRegionAvail();
		glm::vec2 new_viewport_size = { vidget_size.x, vidget_size.y }; // most likely simple cast possible, but still different data types from different librarys

		if (m_ViewportSize != new_viewport_size)
		{
			// there is a bug in ImGui that is causing GetContentRegionAvail() to report wrong values in first frame
			// this is a workaround that prevents creation of framebuffer with 0 hight or with
			if (new_viewport_size.x == 0 || new_viewport_size.y == 0)
				new_viewport_size = { 1, 1 };

			m_Framebuffer->Resize((uint32_t)new_viewport_size.x, (uint32_t)new_viewport_size.y);
			m_ViewportSize = new_viewport_size;
			m_CameraController->Resize(new_viewport_size.x, new_viewport_size.y);
		}

		auto attachment_index = m_Framebuffer->GetColorAttachmentIndex("Final Frame");
		GLuint attachment_id = static_cast<Resource::Framebuffer_OpenGL*>(m_Framebuffer.get())->ColorAttachmentOpenGLIDs[attachment_index];
		ImGui::Image((void*)(uint64_t)attachment_id, vidget_size, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		RenderGuizmos();

		auto viewport_region_min = ImGui::GetWindowContentRegionMin();
		auto viewport_region_max = ImGui::GetWindowContentRegionMax();
		auto viewport_offset = ImGui::GetWindowPos();
		m_ViewportBounds[0] = { viewport_region_min.x + viewport_offset.x, viewport_region_min.y + viewport_offset.y };
		m_ViewportBounds[1] = { viewport_region_max.x + viewport_offset.x, viewport_region_max.y + viewport_offset.y };

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
		FE_PROFILER_FUNC();

		auto scene_observer = m_Scene.Observe();
		auto gameplay_world = scene_observer.GetCoreComponent().GameplayWorld.get();

		Entity selected_entity(m_SelectedEntityID, gameplay_world);

		if (!selected_entity)
			return;

		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();

		ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);

		// Camera
		auto camera_entity = gameplay_world->GetEntityWithPrimaryCamera();
		const auto& camera = m_CameraController->GetCamera();
		const glm::mat4& camera_projection_matrix = camera.GetProjectionMatrix();
		glm::mat4 camera_view_matrix = glm::inverse(m_CameraController->GetTransform().GetMatrix());

		// Entity transform
		glm::mat4 selected_entity_transform_matrix = selected_entity.GetTransformHandle().GetGlobal().GetMatrix();

		// Snapping
		bool snap = InputPolling::IsKeyPressed(InputCodes::LeftControl);
		float snap_value = 0.5f; // Snap to 0.5m for translation/scale
		// Snap to 45 degrees for rotation
		if (m_GuizmoType == ImGuizmo::OPERATION::ROTATE)
			snap_value = 45.0f;

		float snap_values[3] = { snap_value, snap_value, snap_value };

		ImGuizmo::Manipulate(
			glm::value_ptr(camera_view_matrix),
			glm::value_ptr(camera_projection_matrix),
			(ImGuizmo::OPERATION)m_GuizmoType,
			ImGuizmo::LOCAL,
			glm::value_ptr(selected_entity_transform_matrix),
			nullptr,
			snap ? snap_values : nullptr
		);

		if (ImGuizmo::IsUsing())
		{
			Transform transform;
			Math::DecomposeTransform(selected_entity_transform_matrix, transform);
			transform.Rotation = glm::degrees(transform.Rotation);
			selected_entity.GetTransformHandle().SetGlobal(transform);
		}
	}

	EntityID EditViewport::ReadEntityIDfromBuffer()
	{
		FE_PROFILER_FUNC();

		auto [mx, my] = ImGui::GetMousePos();
		mx -= m_ViewportBounds[0].x;
		my -= m_ViewportBounds[0].y;
		glm::vec2 viewport_size = m_ViewportBounds[1] - m_ViewportBounds[0];
		my = viewport_size.y - my;
		int mouseX = (int)mx;
		int mouseY = (int)my;

		EntityID entityID = NullEntityID;

		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewport_size.x && mouseY < (int)viewport_size.y)
		{
			int attachment_index = m_Framebuffer->GetColorAttachmentIndex("EntityID");
			Command::DeviceState::BindFramebuffer<GAPIType::OpenGL>(*m_Framebuffer);
			Command::ResourceState::ReadPixel<GAPIType::OpenGL>(*m_Framebuffer, attachment_index, mouseX, mouseY, &entityID);
		}

		return entityID;
	}
}