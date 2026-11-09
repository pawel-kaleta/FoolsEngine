#include "EditViewport.h"

namespace fe
{
	EditViewport::EditViewport()
	{
		FE_PROFILER_FUNC();

		glm::vec<2, U32> init_size = { 200,200 };

		mFramebuffer = Render::GAPI::CreateFramebufferCmd({
			.mColorFormats =
			{
				Render::GAPI::Descriptors::Texture::Format::RGB_8, 
				Render::GAPI::Descriptors::Texture::Format::R_UINT_32
			},
			.mDepthStencilFormat	= Render::GAPI::Descriptors::Texture::Format::DEPTH32F_STENCIL8,
			.mDepthRange			= Render::GAPI::Descriptors::Raster::DepthRange::ZeroToOne
		}, init_size.x, init_size.y);

		mFramebufferSize = init_size;

		m_CameraController = CreateScope<EditorCameraController>(init_size.x, init_size.y);
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

		if (m_ResizeFramebuffer)
		{
			Render::GAPI::ResizeFramebufferCmd(mFramebuffer, (uint32_t)mFramebufferSize.x, (uint32_t)mFramebufferSize.y);
			m_CameraController->Resize(mFramebufferSize.x, mFramebufferSize.y);
			m_ResizeFramebuffer = false;
		}

		auto scene_observer = m_Scene.Observe();

		Render::Integration::Director::RenderScene(scene_observer, m_CameraController->GetCamera(), m_CameraController->GetTransform(), mFramebuffer);
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
		
		auto attachment_id = Render::GAPI::GetAttachmentNativeID(mFramebuffer, 0);
		//auto attachment_id = Render::GAPI::GetDepthAttachmentNativeID(mFramebuffer);
		//auto attachment_id = Render::GAPI::GetAttachmentNativeID(mFramebuffer, 1);

		ImGui::Image((void*)attachment_id, vidget_size, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });


		glm::vec2 new_framebuffer_size = { vidget_size.x, vidget_size.y };
		if (mFramebufferSize != new_framebuffer_size)
		{
			// there is a bug in ImGui that is causing GetContentRegionAvail() to report wrong values in first frame
			// this is a workaround that prevents creation of framebuffer with 0 hight or with
			if (new_framebuffer_size.x == 0 || new_framebuffer_size.y == 0)
				new_framebuffer_size = { 1, 1 };

			mFramebufferSize = new_framebuffer_size;
			m_ResizeFramebuffer = true;
		}


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
		auto gameplay_world = scene_observer.Get<Scene::Core>().GameplayWorld.get();

		Entity selected_entity(m_SelectedEntityID, gameplay_world);

		if (!selected_entity)
			return;

		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();

		ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);

		// Camera
		auto camera_entity = gameplay_world->GetEntityWithPrimaryCamera();
		const auto& camera = m_CameraController->GetCamera();

		//const glm::mat4& camera_projection_matrix = camera.GetProjectionMatrix();
		const glm::mat4& camera_projection_matrix = glm::perspective(camera.m_PerspectiveData.m_FOV, camera.m_AspectRatio, camera.m_PerspectiveData.m_NearClip, camera.m_PerspectiveData.m_FarClip);

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
			String attachment_name; attachment_name.FromConstCharPtr("EntityID", 9);

			Context::Log::Core.Error("EditViewport::ReadEntityIDfromBuffer() not implemented");

			//int attachment_index = m_Framebuffer->GetColorAttachmentIndex(attachment_name);
			//Command::PipelineState::BindFramebuffer_OpenGL(*(Resource::RFramebuffer_OpenGL*) & *m_Framebuffer);
			Splice<Byte> entityID_memReg;
			entityID_memReg.Elements = (Byte*) & entityID;
			entityID_memReg.Count = sizeof(EntityID);
			//m_Framebuffer->ReadPixel(attachment_index, mouseX, mouseY, entityID_memReg);
		}

		return entityID;
	}
}