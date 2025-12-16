#include "PlayViewport.h"

namespace fe
{
	PlayViewport::PlayViewport()
	{
		FE_PROFILER_FUNC();

		//Description::Framebuffer::SpecificationBuilder spec_builder;
		//spec_builder
		//	.SetWidth(1)
		//	.SetHight(1)
		//	.SetDepthStencilAttachmentFormat(Description::Texture::Format::DEPTH24STENCIL8)
		//	.AddColorAttachmentSpecification(Description::Framebuffer::Attachment("Final Frame", Description::Texture::Format::RGBA_8))
		//	.AddColorAttachmentSpecification(Description::Framebuffer::Attachment("EntityID"   , Description::Texture::Format::R_UINT_32));
		//m_Framebuffer = Framebuffer::Create(spec_builder.Create());

		m_Framebuffer.reset(new Resource::Framebuffer_OpenGL());

		auto& lib = Description::Library::Get();
		m_Framebuffer->SpecificationID = lib.FramebufferSpecs.size();
		auto& framebuffer_spec = lib.FramebufferSpecs.emplace_back();
		framebuffer_spec.Width = 1;
		framebuffer_spec.Height = 1;
		framebuffer_spec.DepthStencilFormat = Description::Texture::Format::DEPTH24STENCIL8;
		framebuffer_spec.ColorAttachments.emplace_back("Final Frame", Description::Texture::Format::RGBA_8);
		framebuffer_spec.Type = Description::Texture::Type::Texture2D;

		m_Framebuffer->Create();


		m_ViewportSize = { 1,1 };
	}

	void PlayViewport::RenderScene()
	{
		FE_PROFILER_FUNC();

		if (!m_IsVisible)
			return;

		auto scene_observer = m_Scene.Observe();
		Entity camera_entity = scene_observer.GetCoreComponent().GameplayWorld->GetEntityWithPrimaryCamera();
		if (camera_entity)
		{
			auto& camera_component = camera_entity.Get<CCamera>();
			auto& camera = camera_component.Camera;
			auto camera_transform = camera_entity.GetTransformHandle().GetGlobal();
			camera_transform.Scale = { 1.f,1.f,1.f };
			camera_transform = camera_transform + camera_component.Offset;
			Renderer::RenderScene(scene_observer, camera, camera_transform, *m_Framebuffer.get());
		}
		else
		{
			Command::DeviceState::BindFramebuffer<GAPIType::OpenGL>(*m_Framebuffer);
			Command::ResourceState::Clear<GAPIType::OpenGL>();
		}
		
	}

	void PlayViewport::OnImGuiRender()
	{
		FE_PROFILER_FUNC();

		ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		m_IsVisible = ImGui::Begin("Play");
		ImGui::PopStyleVar();

		m_VieportFocus = ImGui::IsWindowFocused();
		m_VieportHover = ImGui::IsWindowHovered();

		Application::Get().m_ImGuiLayer->BlockEvents = !(m_VieportFocus || m_VieportHover) && m_IsVisible;

		auto vidget_size = ImGui::GetContentRegionAvail();
		glm::vec2 new_viewport_size = { vidget_size.x, vidget_size.y }; // most likely simple cast possible, but still different data types from different libraries

		if (m_ViewportSize != new_viewport_size)
		{
			// there is a bug in ImGui that is causing GetContentRegionAvail() to report wrong values in first frame
			// this is a workaround that prevents creation of framebuffer with 0 hight or with
			if (new_viewport_size.x == 0 || new_viewport_size.y == 0)
				new_viewport_size = { 1, 1 };

			auto camera_entity = m_Scene.Observe().GetCoreComponent().GameplayWorld->GetEntityWithPrimaryCamera();
			if (camera_entity)
			{
				auto& camera_component = camera_entity.Get<CCamera>();
				camera_component.Camera.SetViewportSize((uint32_t)new_viewport_size.x, (uint32_t)new_viewport_size.y);
			}
			m_Framebuffer->Resize((uint32_t)new_viewport_size.x, (uint32_t)new_viewport_size.y);
			m_ViewportSize = new_viewport_size;
		}

		auto attachment_index = m_Framebuffer->GetColorAttachmentIndex("Final Frame");
		GLuint attachment_id = static_cast<Resource::Framebuffer_OpenGL*>(m_Framebuffer.get())->ColorAttachmentOpenGLIDs[attachment_index];
		ImGui::Image((void*)(uint64_t)attachment_index, vidget_size, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		ImGui::End();
	}

	void PlayViewport::OnScenePlayStop()
	{
		FE_PROFILER_FUNC();

		auto camera_entity = m_Scene.Observe().GetCoreComponent().GameplayWorld->GetEntityWithPrimaryCamera();
		if (camera_entity)
		{
			auto& camera_component = camera_entity.Get<CCamera>();
			camera_component.Camera.SetViewportSize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}
	}
}