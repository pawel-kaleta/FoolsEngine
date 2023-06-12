#include "LayerExample.h"

LayerExample::LayerExample()
	: fe::Layer("LayerExample")
{ }

void LayerExample::OnAttach()
{
	m_Scene = fe::CreateScope<fe::Scene>();

	auto camera = m_Scene->CreateSet();
	camera.Emplace<fe::CTransform>();
	camera.Emplace<fe::CCamera>(fe::CCamera::Orthographic, 1280.0f / 720.0f);
	m_Scene->SetPrimaryCameraSet(camera);

	fe::Set tintedTextureQuad = m_Scene->CreateSet();
	{
		auto& quad = tintedTextureQuad.Emplace<fe::Renderer2D::Quad>();
		quad.Texture = fe::TextureLibrary::Get("Default_Texture");
		quad.Color = glm::vec4(0.1f, 0.1f, 1.0f, 1.0f);
		quad.Layer = fe::Renderer2D::Layer::L_2;
		quad.TextureTilingFactor = 3;
		quad.Transparency = false;

		auto& transform = tintedTextureQuad.Emplace<fe::CTransform>();
		transform.Scale = glm::vec3(0.6f, 0.4f, 1.0f);
		transform.Rotation = glm::vec3(0.0f, 0.0f, 20.0f);
	}

	m_ColorQuad = m_Scene->CreateSet();
	{
		auto& quad = m_ColorQuad.Emplace<fe::Renderer2D::Quad>();
		quad.Color = glm::vec4(0.9f, 0.2f, 0.9f, 0.8f);
		quad.Layer = fe::Renderer2D::Layer::L_1;

		auto& transform = m_ColorQuad.Emplace<fe::CTransform>();
		transform.Position = glm::vec3(-0.1f, -0.1f, 0.0f);
		transform.Scale = glm::vec3(0.3f, 0.2f, 1.0f);
	}

	m_Target = m_Scene->CreateSet();
	{
		fe::TextureLibrary::Add(fe::Texture2D::Create("assets/textures/Texture_with_Transparency.png"));

		auto& quad = m_Target.Emplace<fe::Renderer2D::Quad>();
		quad.Layer = fe::Renderer2D::Layer::L0;
		quad.Texture = fe::TextureLibrary::Get("Texture_with_Transparency");

		auto& transform = m_Target.Emplace<fe::CTransform>();
		transform.Position = glm::vec3(0.0f, 0.0f, 0.0f);
		transform.Scale = glm::vec3(0.3f, 0.3f, 1.0f);
	}
}

void LayerExample::OnUpdate()
{
	FE_PROFILER_FUNC();
	FE_LOG_TRACE("LayerExample::OnUpdate()");

	float moveDistance = fe::Time::DeltaTime() * m_TargetMoveSpeed;

	auto& targetPosition = m_Target.Get<fe::CTransform>().Position;

	     if (fe::InputPolling::IsKeyPressed(fe::InputCodes::Right))	targetPosition.x += moveDistance;
	else if (fe::InputPolling::IsKeyPressed(fe::InputCodes::Left))	targetPosition.x -= moveDistance;
	     if (fe::InputPolling::IsKeyPressed(fe::InputCodes::Up))	targetPosition.y += moveDistance;
	else if (fe::InputPolling::IsKeyPressed(fe::InputCodes::Down))	targetPosition.y -= moveDistance;

	fe::Renderer2D::RenderScene(*m_Scene, m_Scene->GetSetWithPrimaryCamera());
}

void LayerExample::OnImGuiRender()
{
	ImGui::Begin("Settings");
	
	ImGui::ColorEdit4("Quad color", (float *)& m_ColorQuad.Get<fe::Renderer2D::Quad>().Color);

	ImGui::Text("Stats:");
	auto& stats = fe::Renderer2D::GetStats();
	ImGui::Text("Draw Calls: %d", stats.DrawCalls);
	ImGui::Text("Quads: %d", stats.Quads);
	ImGui::Text("Render Time: %F", stats.RenderTime.GetMilliseconds());
	ImGui::Text("Frame Time: %F", fe::Time::DeltaTime() * 1000);
	ImGui::Text("FPS: %F", 1.0f / fe::Time::DeltaTime());
	ImGui::End();
	
}

void LayerExample::OnEvent(fe::Ref<fe::Events::Event> event)
{
	FE_LOG_TRACE("{0}", event);

	fe::Events::EventDispacher dispacher(event);
	dispacher.Dispach<fe::Events::KeyPressedEvent>(FE_BIND_EVENT_HANDLER(LayerExample::OnKeyPressedEvent));
}

void LayerExample::OnKeyPressedEvent(fe::Ref<fe::Events::KeyPressedEvent> event)
{
	
}
