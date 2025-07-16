#include "LayerExample.h"

LayerExample::LayerExample()
	: fe::Layer("LayerExample")
{ }

void LayerExample::OnAttach()
{
	auto sceneID = fe::AssetManager::GetOrCreateAssetWithUUID(fe::Project::GetInstance()->StartScene);
	m_Scene = fe::AssetHandle<fe::Scene>(sceneID, fe::AssetLoadingPriority::Critical);
	{
		auto scene_user = m_Scene.Use();
		scene_user.Initialize();
		auto success = fe::SceneSerializerYAML::DeserializeFromFile(scene_user);
		FE_ASSERT(success, "scene loading failed");
	}

	auto& window = fe::Application::GetWindow();

	fe::FramebufferData::SpecificationBuilder specBuilder;
	specBuilder
		.SetWidth(window.GetWidth())
		.SetHight(window.GetHeight())
		.SetDepthStencilAttachmentDataFormat(fe::TextureData::Format::DEPTH24STENCIL8)
		.SetColorAttachmentSpecifications({
			{ "Final Frame", fe::TextureData::Components::RGBA, fe::TextureData::Format::RGBA_8		},
			{ "EntityID"   , fe::TextureData::Components::R   , fe::TextureData::Format::R_UINT_32	}
			});
	m_Framebuffer = fe::Framebuffer::Create(specBuilder.Create());
	m_FramebufferSize = { window.GetWidth(), window.GetHeight() };
}

void LayerExample::OnUpdate()
{
	FE_PROFILER_FUNC();
	FE_LOG_TRACE("LayerExample::OnUpdate()");

	{
		auto scene_user = m_Scene.Use();
		scene_user.SimulationUpdate();
		scene_user.PostFrameUpdate();
	}

	fe::AssetManager::EvaluateAndReload();

	auto scene_observer = m_Scene.Observe();
	fe::Entity camera_entity = scene_observer.GetCoreComponent().GameplayWorld->GetEntityWithPrimaryCamera();

	if (camera_entity)
	{
		auto& cameraComponent = camera_entity.Get<fe::CCamera>();
		auto& camera = cameraComponent.Camera;

		auto& window = fe::Application::GetWindow();
		uint32_t new_x = window.GetWidth();
		uint32_t new_y = window.GetHeight();

		if (m_FramebufferSize.x != new_x || m_FramebufferSize.y != new_y)
		{
			camera.SetViewportSize(new_x, new_y);
			m_Framebuffer->Resize(new_x, new_y);
			m_FramebufferSize = { new_x, new_y };
		}

		auto cameraTransform = camera_entity.GetTransformHandle().GetGlobal();
		cameraTransform.Scale = { 1.f,1.f,1.f };
		cameraTransform = cameraTransform + cameraComponent.Offset;
		fe::Renderer::RenderScene(scene_observer, camera, cameraTransform, *m_Framebuffer.get());
	}
}

void LayerExample::OnImGuiRender()
{
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	ImGuiWindowFlags windowFlags = 
		ImGuiWindowFlags_::ImGuiWindowFlags_NoDocking  |
		ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_::ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_::ImGuiWindowFlags_NoMove |

		ImGuiWindowFlags_::ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_::ImGuiWindowFlags_NoNavFocus;

	ImGui::Begin("Main", nullptr, windowFlags);

	ImGui::PopStyleVar(3);

	auto fbID = m_Framebuffer->GetColorAttachmentID();
	auto vidgetSize = ImGui::GetContentRegionAvail();
	ImGui::Image((void*)(uint64_t)fbID, vidgetSize, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

	ImGui::End();


	ImGui::Begin("Stats");

	auto stats = fe::Renderer2D::GetStats();
	ImGui::Text("Draw Calls: %d", stats.DrawCalls);
	ImGui::Text("Quads: %d", stats.Quads);
	ImGui::Text("Render Time: %F", stats.RenderTime.GetMilliseconds());
	ImGui::Text("Frame Time: %F", fe::Time::DeltaTime() * 1000);
	ImGui::Text("FPS: %F", 1.0f / fe::Time::DeltaTime());
	ImGui::End();
}

void LayerExample::OnEvent(fe::Ref<fe::Events::Event> event)
{
	FE_LOG_TRACE("{0}", *event.get());

	fe::Events::EventDispacher dispacher(event);
	dispacher.Dispach<fe::Events::KeyPressedEvent>(FE_BIND_EVENT_HANDLER(LayerExample::OnKeyPressedEvent));
}

void LayerExample::OnKeyPressedEvent(fe::Ref<fe::Events::KeyPressedEvent> event)
{
	
}
