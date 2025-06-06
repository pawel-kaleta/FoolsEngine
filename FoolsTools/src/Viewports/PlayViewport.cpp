#include "PlayViewport.h"

#include "FoolsEngine\Renderer\4 - GDIIsolation\RenderCommands.h"

namespace fe
{
	PlayViewport::PlayViewport()
	{
		FE_PROFILER_FUNC();

		FramebufferData::SpecificationBuilder specBuilder;
		specBuilder
			.SetWidth(1)
			.SetHight(1)
			.SetDepthStencilAttachmentDataFormat(TextureData::Format::DEPTH24STENCIL8)
			.SetColorAttachmentSpecifications({
				{ "Final Frame", TextureData::Components::RGBA, TextureData::Format::RGBA_8		},
				{ "EntityID"   , TextureData::Components::R   , TextureData::Format::R_UINT_32	}
			});
		m_Framebuffer = Framebuffer::Create(specBuilder.Create());
		m_ViewportSize = { 1,1 };
	}

	void PlayViewport::RenderScene()
	{
		FE_PROFILER_FUNC();

		if (!m_IsVisible)
			return;

		auto scene_observer = m_Scene.Observe();
		Entity cameraEntity = scene_observer.GetCoreComponent().GameplayWorld->GetEntityWithPrimaryCamera();
		if (cameraEntity)
		{
			auto& cameraComponent = cameraEntity.Get<CCamera>();
			auto& camera = cameraComponent.Camera;
			auto cameraTransform = cameraEntity.GetTransformHandle().GetGlobal();
			cameraTransform.Scale = { 1.f,1.f,1.f };
			cameraTransform = cameraTransform + cameraComponent.Offset;
			Renderer::RenderScene(scene_observer, camera, cameraTransform, *m_Framebuffer.get());
		}
		else
		{
			m_Framebuffer->Bind();
			RenderCommands::Clear();
			m_Framebuffer->Unbind();
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

		Application::GetImguiLayer()->BlockEvents(!(m_VieportFocus || m_VieportHover) && m_IsVisible);

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
		}

		auto fbID = m_Framebuffer->GetColorAttachmentID();
		ImGui::Image((void*)(uint64_t)fbID, vidgetSize, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		ImGui::End();
	}
}