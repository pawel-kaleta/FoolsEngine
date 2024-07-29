#include "LayerExample.h"

LayerExample::LayerExample()
	: fe::Layer("LayerExample")
{ }

/*
struct MovementSystem : fe::Behavior
{
	float MoveSpeed = 0.5f;


	void OnUpdate_Physics() override
	{
		auto entity = fe::Entity(GetHeadEntityID(), GetScene());
		auto transform = entity.GetTransformHandle();
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
};*/

void LayerExample::OnAttach()
{
	m_Scene = fe::CreateRef<fe::Scene>();

	auto camera = m_Scene->GetGameplayWorld()->CreateActor();
	{
		camera.Emplace<fe::CCamera>();
		fe::Transform transform;
		transform.Shift = glm::vec3(0.0f, 0.0f, 2.0f);
		camera.GetTransformHandle() = transform;
	}
	m_Scene->GetGameplayWorld()->SetPrimaryCameraEntity(camera);

	fe::Entity tintedTextureTile = m_Scene->GetGameplayWorld()->CreateActor();
	{
		auto& tile = tintedTextureTile.Emplace<fe::CTile>().Tile;
		tile.Texture = fe::TextureLibrary::Get("Default_Texture");
		tile.Color = glm::vec4(0.2f, 0.7f, 0.3f, 1.0f);
		tile.TextureTilingFactor = 3;

		fe::Transform transform;
		transform.Scale = glm::vec3(0.6f, 0.4f, 1.0f);
		transform.Rotation = glm::vec3(0.0f, 0.0f, -30.0f);
		transform.Shift = glm::vec3(0.0f, 0.2f, 0.0f);
		tintedTextureTile.GetTransformHandle() = transform;
	}

	fe::Entity flatTile = m_Scene->GetGameplayWorld()->CreateActor();
	{
		auto& tile = flatTile.Emplace<fe::CTile>().Tile;
		tile.Color = glm::vec4(0.1f, 0.1f, 1.0f, 1.0f);
		tile.TextureTilingFactor = 3;

		fe::Transform transform;
		transform.Scale = glm::vec3(0.4f, 0.3f, 1.0f);
		transform.Rotation = glm::vec3(0.0f, 0.0f, 20.0f);
		flatTile.GetTransformHandle() = transform;
	}

	m_ColorSprite = m_Scene->GetGameplayWorld()->CreateActor();
	{
		auto& sprite = m_ColorSprite.Emplace<fe::CSprite>().Sprite;
		sprite.Color = glm::vec4(0.9f, 0.2f, 0.9f, 0.8f);

		fe::Transform transform;
		transform.Shift = glm::vec3(-0.1f, -0.1f, 0.1f);
		transform.Scale = glm::vec3(0.3f, 0.2f, 1.0f);
		m_ColorSprite.GetTransformHandle() = transform;
	}

	fe::Entity target = m_Scene->GetGameplayWorld()->CreateEntity(fe::RootID, "Target");
	{
		fe::TextureLibrary::Add(fe::Texture2D::Create("assets/textures/Texture_with_Transparency.png"));

		auto& sprite = target.Emplace<fe::CSprite>().Sprite;
		sprite.Texture = fe::TextureLibrary::Get("Texture_with_Transparency");

		fe::Transform transform;
		transform.Shift = glm::vec3(0.0f, 0.0f, 0.2f);
		transform.Scale = glm::vec3(0.3f, 0.3f, 1.0f);
		target.GetTransformHandle() = transform;

		//target.AddGOController<MovementSystem>();

		auto tags = target.GetTagsHandle().Local();
		tags += fe::Tags::Player;
		target.GetTagsHandle().SetLocal(tags);
	}

	fe::Entity targetChild_1 = m_Scene->GetGameplayWorld()->CreateEntity(target, "TargetChild");
	{
		auto& sprite = targetChild_1.Emplace<fe::CSprite>().Sprite;
		sprite.Texture = fe::TextureLibrary::Get("Texture_with_Transparency");
		sprite.Color = { 1.0f, 1.0f, 1.0f, 0.5f };

		fe::Transform transform;
		transform.Shift = glm::vec3(0.8f, 0.8f, 0.3f);
		transform.Rotation = glm::vec3(0.0f, 0.0f, 20.0f);
		transform.Scale = glm::vec3(0.5f, 0.5f, 1.0f);
		targetChild_1.GetTransformHandle().SetLocal(transform);
	}
}

void LayerExample::OnUpdate()
{
	FE_PROFILER_FUNC();
	FE_LOG_TRACE("LayerExample::OnUpdate()");

	m_Scene->GetGameplayWorld()->GetHierarchy().MakeGlobalTransformsCurrent();
	fe::Renderer2D::RenderScene(*m_Scene, m_Scene->GetGameplayWorld()->GetEntityWithPrimaryCamera());
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
