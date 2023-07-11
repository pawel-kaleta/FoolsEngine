#include "LayerExample.h"

LayerExample::LayerExample()
	: fe::Layer("LayerExample")
{ }

struct TargetBehaviourScriptExample : fe::NativeScript
{
	float MoveSpeed = 0.5f;

	void OnUpdate() override
	{
		auto transform = GetTransformHandle();
		auto newTransform = transform.Global();
		auto& position = newTransform.Position;
		auto& rotation = newTransform.Rotation;
		auto& scale = newTransform.Scale;


		float moveDistance = fe::Time::DeltaTime() * MoveSpeed;
		float rotSpeed     = fe::Time::DeltaTime() * 80.0f;
		float scaleSpeed   = fe::Time::DeltaTime() * 0.2f;

		     if (fe::InputPolling::IsKeyPressed(fe::InputCodes::KP1))	scale -= scaleSpeed;
		else if (fe::InputPolling::IsKeyPressed(fe::InputCodes::KP3))	scale += scaleSpeed;

		     if (fe::InputPolling::IsKeyPressed(fe::InputCodes::KP4))	rotation.z -= rotSpeed;
		else if (fe::InputPolling::IsKeyPressed(fe::InputCodes::KP6))	rotation.z += rotSpeed;

		     if (fe::InputPolling::IsKeyPressed(fe::InputCodes::Right))	position.x += moveDistance;
		else if (fe::InputPolling::IsKeyPressed(fe::InputCodes::Left))	position.x -= moveDistance;
		     if (fe::InputPolling::IsKeyPressed(fe::InputCodes::Up))	position.y += moveDistance;
		else if (fe::InputPolling::IsKeyPressed(fe::InputCodes::Down))	position.y -= moveDistance;

		transform = newTransform;
	}
};

void LayerExample::OnAttach()
{
	m_Scene = fe::CreateRef<fe::Scene>();

	auto camera = m_Scene->CreateSet();
	camera.Emplace<fe::CCamera>(fe::CCamera::Orthographic, 1280.0f / 720.0f);
	m_Scene->SetPrimaryCameraSet(camera);

	fe::Set tintedTextureTile = m_Scene->CreateSet();
	{
		auto& tile = tintedTextureTile.Emplace<fe::Renderer2D::CTile>();
		tile.Texture = fe::TextureLibrary::Get("Default_Texture");
		tile.Color = glm::vec4(0.2f, 0.7f, 0.3f, 1.0f);
		tile.TextureTilingFactor = 3;

		fe::Transform transform;
		transform.Scale = glm::vec3(0.6f, 0.4f, 1.0f);
		transform.Rotation = glm::vec3(0.0f, 0.0f, -30.0f);
		transform.Position = glm::vec3(0.0f, 0.2f, 0.0f);
		tintedTextureTile.GetTransformHandle() = transform;
	}

	fe::Set flatTile = m_Scene->CreateSet();
	{
		auto& tile = flatTile.Emplace<fe::Renderer2D::CTile>();
		tile.Color = glm::vec4(0.1f, 0.1f, 1.0f, 1.0f);
		tile.TextureTilingFactor = 3;

		fe::Transform transform;
		transform.Scale = glm::vec3(0.4f, 0.3f, 1.0f);
		transform.Rotation = glm::vec3(0.0f, 0.0f, 20.0f);
		flatTile.GetTransformHandle() = transform;
	}

	m_ColorSprite = m_Scene->CreateSet();
	{
		auto& sprite = m_ColorSprite.Emplace<fe::Renderer2D::CSprite>();
		sprite.Color = glm::vec4(0.9f, 0.2f, 0.9f, 0.8f);

		fe::Transform transform;
		transform.Position = glm::vec3(-0.1f, -0.1f, 0.1f);
		transform.Scale = glm::vec3(0.3f, 0.2f, 1.0f);
		m_ColorSprite.GetTransformHandle() = transform;
	}

	fe::Set target = m_Scene->CreateSet(fe::RootID, "Target");
	{
		fe::TextureLibrary::Add(fe::Texture2D::Create("assets/textures/Texture_with_Transparency.png"));

		auto& sprite = target.Emplace<fe::Renderer2D::CSprite>();
		sprite.Texture = fe::TextureLibrary::Get("Texture_with_Transparency");

		fe::Transform transform;
		transform.Position = glm::vec3(0.0f, 0.0f, 0.2f);
		transform.Scale = glm::vec3(0.3f, 0.3f, 1.0f);
		target.GetTransformHandle() = transform;

		target.AddScript<TargetBehaviourScriptExample>();

		auto tags = target.GetTagsHandle().Local();
		tags.Common.Add(fe::CommonTags::Player);
		target.GetTagsHandle().SetLocal(tags);
	}

	fe::Set targetChild_1 = m_Scene->CreateSet(target, "TargetChild");
	{
		auto& sprite = targetChild_1.Emplace<fe::Renderer2D::CSprite>();
		sprite.Texture = fe::TextureLibrary::Get("Texture_with_Transparency");
		sprite.Color = { 1.0f, 1.0f, 1.0f, 0.5f };

		fe::Transform transform;
		transform.Position = glm::vec3(0.8f, 0.8f, 0.3f);
		transform.Rotation = glm::vec3(0.0f, 0.0f, 20.0f);
		transform.Scale = glm::vec3(0.5f, 0.5f, 1.0f);
		targetChild_1.GetTransformHandle().SetLocal(transform);
	}
}

void LayerExample::OnUpdate()
{
	FE_PROFILER_FUNC();
	FE_LOG_TRACE("LayerExample::OnUpdate()");

	m_Scene->UpdateScripts();
	m_Scene->DestroyFlaggedSets();
	m_Scene->GetHierarchy().MakeGlobalTransformsCurrent();
	fe::Renderer2D::RenderScene(*m_Scene, m_Scene->GetSetWithPrimaryCamera());
}

void LayerExample::OnImGuiRender()
{
	ImGui::Begin("Stats");

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
