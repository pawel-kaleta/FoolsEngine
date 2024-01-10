#include "EditorLayer.h"
#include "FoolsEngine\Scene\Behavior.h"
#include "FoolsEngine\Scene\Actor.h"
#include "FoolsEngine\Scene\CompPtr.h"

namespace fe
{
	EditorLayer::EditorLayer()
		: Layer("EditorLayer")
	{
	}

	struct CPlayerMovement : DataComponent
	{
		struct TargetMovement {
			float MoveSpeed = 0.5f;
			float RotationSpeed = 80.0f;
			float ScaleStepSpeed = 0.2f;

			Transform CalculateNewTransform(Transform transform)
			{
				auto& position = transform.Position;
				auto& rotation = transform.Rotation;
				auto& scale    = transform.Scale;

				float moveDistance = Time::DeltaTime() * MoveSpeed;
				float rotAngle     = Time::DeltaTime() * RotationSpeed;
				float scaleStep    = Time::DeltaTime() * ScaleStepSpeed;

					 if (InputPolling::IsKeyPressed(InputCodes::KP1))	scale -= scaleStep;
				else if (InputPolling::IsKeyPressed(InputCodes::KP3))	scale += scaleStep;

					 if (InputPolling::IsKeyPressed(InputCodes::KP4))	rotation.z -= rotAngle;
				else if (InputPolling::IsKeyPressed(InputCodes::KP6))	rotation.z += rotAngle;

					 if (InputPolling::IsKeyPressed(InputCodes::Right))	position.x += moveDistance;
				else if (InputPolling::IsKeyPressed(InputCodes::Left))	position.x -= moveDistance;
					 if (InputPolling::IsKeyPressed(InputCodes::Up))	position.y += moveDistance;
				else if (InputPolling::IsKeyPressed(InputCodes::Down))	position.y -= moveDistance;

				return transform;
			}
		} PlayerMovement;

		FE_COMPONENT_SETUP(CPlayerMovement, "PlayerMovement");
		virtual void DrawInspectorWidget(BaseEntity entity) override
		{
			ImGui::DragFloat("MoveSpeed"     , &PlayerMovement.MoveSpeed     , 0.01f);
			ImGui::DragFloat("RotationSpeed" , &PlayerMovement.RotationSpeed , 0.1f);
			ImGui::DragFloat("ScaleStepSpeed", &PlayerMovement.ScaleStepSpeed, 0.01f);
		}
	};

	class PlayerMovementBehavior : public Behavior
	{
	public:
		//PlayerMovementBehavior() = default;
		virtual ~PlayerMovementBehavior() override = default;
	
		virtual void OnUpdate_PrePhysics() override
		{
			auto transform = m_Player.GetTransformHandle();
			auto newTransform = m_Movement.Get()->PlayerMovement.CalculateNewTransform(transform.Global());
			transform = newTransform;
		}

		virtual void OnInitialize() override
		{
			RegisterForUpdate<SimulationStages::PrePhysics>();
		}

		virtual void DrawInspectorWidget() const override 
		{
			DrawCompPtr(m_Movement, "Movement Component");
			DrawEntity(m_Player, "Player's root");
		}

		FE_BEHAVIOR_SETUP(PlayerMovementBehavior, "PlayerMovement");

		CompPtr<CPlayerMovement> m_Movement;
		Entity m_Player;
	};

	void EditorLayer::OnAttach()
	{
		FE_PROFILER_FUNC();
		FE_LOG_INFO("EditorLayer::OnAttach()");

		FramebufferData::Specification fbSpec;
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_Framebuffer = Framebuffer::Create(fbSpec);

		m_CameraController = CreateScope<EditorCameraController>(1280.0f, 720.0f);

		m_Scene = CreateRef<Scene>();
		m_SceneHierarchyPanel.SetScene(m_Scene);
		m_EntityInspector.SetScene(m_Scene);

		TestSceneSetup();
	}

	void EditorLayer::OnUpdate()
	{
		FE_PROFILER_FUNC();
		FE_LOG_TRACE("EditorLayer::OnUpdate()");

		if (m_VieportFocus)
		{
			m_Scene->SimulationUpdate();

			m_CameraController->OnUpdate();
		}
		m_Scene->PostFrameUpdate();

		m_Scene->GetGameplayWorld()->GetHierarchy().MakeGlobalTransformsCurrent();

		m_Framebuffer->Bind();
		Renderer2D::RenderScene(*m_Scene, m_CameraController->GetCamera(), m_CameraController->GetTransform());
		m_Framebuffer->Unbind();
	}

	void EditorLayer::OnImGuiRender()
	{
		FE_PROFILER_FUNC();

		static bool dockspaceOpen = true;
		static bool constFullscreenOpt = true;
		bool fullscreenOpt = constFullscreenOpt;
		static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_::ImGuiDockNodeFlags_None;

		// nested docking spaces of the same size bad -> no docking to window, only to dedicated dockspace
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_::ImGuiWindowFlags_NoDocking;
		if (fullscreenOpt)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowBorderSize, 0.0f);

			windowFlags |=
				ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar |
				ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse |
				ImGuiWindowFlags_::ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_::ImGuiWindowFlags_NoMove |
			
				ImGuiWindowFlags_::ImGuiWindowFlags_NoBringToFrontOnFocus |
				ImGuiWindowFlags_::ImGuiWindowFlags_NoNavFocus;
		}

		if (dockspaceFlags & ImGuiDockNodeFlags_::ImGuiDockNodeFlags_PassthruCentralNode)
			windowFlags |= ImGuiWindowFlags_::ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Dockspace", &dockspaceOpen, windowFlags);
		{
			ImGui::PopStyleVar();

			if (fullscreenOpt)
				ImGui::PopStyleVar(2);

			ImGuiIO& io = ImGui::GetIO();
			ImGuiStyle& style = ImGui::GetStyle();
			float minWinSizeX = style.WindowMinSize.x;
			style.WindowMinSize.x = 300.0f;
			if (io.ConfigFlags & ImGuiConfigFlags_::ImGuiConfigFlags_DockingEnable)
			{
				ImGuiID dockspaceID = ImGui::GetID("MyDockSpace");
				ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspaceFlags);
			}
			style.WindowMinSize.x = minWinSizeX;

			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("Exit"))
						Application::Get().Close();

					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}

			m_SceneHierarchyPanel.SetSelection(m_SelectedEntityID);
			m_SceneHierarchyPanel.OnImGuiRender();
			m_SelectedEntityID = m_SceneHierarchyPanel.GetSelection();

			m_EntityInspector.OpenEntity(m_SelectedEntityID);
			m_EntityInspector.OnImGuiRender();

			ImGui::Begin("RenderStats");
			{
				auto& stats = Renderer2D::GetStats();
				ImGui::Text("Draw Calls: %d", stats.DrawCalls);
				ImGui::Text("Quads: %d", stats.Quads);
				ImGui::Text("Render Time: %F", stats.RenderTime.GetMilliseconds());
				ImGui::Text("Frame Time: %F", Time::DeltaTime() * 1000);
				ImGui::Text("FPS: %F", 1.0f / Time::DeltaTime());
			}
			ImGui::End();

			ImGui::Begin("Settings");
			{
				if (ImGui::CollapsingHeader("Editor Camera", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::PushItemWidth(-ImGui::GetContentRegionAvail().x * 0.5f);

					auto& transform = m_CameraController->GetTransform();

					ImGui::DragFloat3("Position", glm::value_ptr(transform.Position), 0.01f);
					ImGui::DragFloat3("Rotation", glm::value_ptr(transform.Rotation), 0.1f);
					ImGui::DragFloat3("Scale", glm::value_ptr(transform.Scale), 0.01f);

					auto& camera = m_CameraController->GetCamera();
					constexpr char* projectionTypeStrings[] = { "Orthographic", "Perspective" };
					const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];

					if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
					{
						for (int i = 0; i < 2; i++)
						{
							bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
							if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
							{
								currentProjectionTypeString = projectionTypeStrings[i];
								camera.SetProjectionType((Camera::ProjectionType)i);
							}

							if (isSelected)
								ImGui::SetItemDefaultFocus();
						}

						ImGui::EndCombo();
					}

					if (camera.GetProjectionType() == Camera::ProjectionType::Perspective)
					{
						float verticalFov = glm::degrees(camera.GetPerspectiveFOV());
						if (ImGui::DragFloat("Field of View", &verticalFov))
							camera.SetPerspectiveFOV(glm::radians(verticalFov));

						float orthoNear = camera.GetPerspectiveNearClip();
						if (ImGui::DragFloat("Near Clip", &orthoNear))
							camera.SetPerspectiveNearClip(orthoNear);

						float orthoFar = camera.GetPerspectiveFarClip();
						if (ImGui::DragFloat("Far Clip", &orthoFar))
							camera.SetPerspectiveFarClip(orthoFar);
					}
					else
					{
						float zoom = camera.GetOrthographicZoom();
						if (ImGui::DragFloat("Zoom", &zoom))
							camera.SetOrthographicZoom(zoom);

						float orthoNear = camera.GetOrthographicNearClip();
						if (ImGui::DragFloat("Near Clip", &orthoNear))
							camera.SetOrthographicNearClip(orthoNear);

						float orthoFar = camera.GetOrthographicFarClip();
						if (ImGui::DragFloat("Far Clip", &orthoFar))
							camera.SetOrthographicFarClip(orthoFar);
					}

					
				}
			}
			ImGui::End();

			ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin("Vieport");
			{
				ImGui::PopStyleVar();

				m_VieportFocus = ImGui::IsWindowFocused();
				m_VieportHover = ImGui::IsWindowHovered();
				Application::Get().GetImguiLayer()->BlockEvents(!(m_VieportFocus && m_VieportHover));

				auto& vidgetSize = ImGui::GetContentRegionAvail();
				glm::vec2 newViewPortSize = { vidgetSize.x, vidgetSize.y };
				
				if (m_ViewportSize != newViewPortSize)
				{
					m_Framebuffer->Resize((uint32_t)newViewPortSize.x, (uint32_t)newViewPortSize.y);
					m_ViewportSize = newViewPortSize;
					m_CameraController->Resize(newViewPortSize.x, newViewPortSize.y);
				}

				auto fbID = m_Framebuffer->GetColorAttachmentID();
				ImGui::Image((void*)(uint64_t)fbID, vidgetSize, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
			}
			ImGui::End();

		}
		ImGui::End();	
	}

	void EditorLayer::TestSceneSetup()
	{
		FE_PROFILER_FUNC();
		FE_LOG_INFO("Test Entities Spawn");

		Actor enviroActor = m_Scene->GetGameplayWorld()->CreateActor("Enviro");
		
		Entity tintedTextureTile = enviroActor.CreateChildEntity("TestEntity");
		{
			auto& tile = tintedTextureTile.Emplace<CTile>().Tile;
			tile.Texture = TextureLibrary::Get("Default_Texture");
			tile.Color = glm::vec4(0.2f, 0.7f, 0.3f, 1.0f);
			tile.TextureTilingFactor = 3;

			Transform transform;
			transform.Scale = glm::vec3(0.6f, 0.4f, 1.0f);
			transform.Rotation = glm::vec3(0.0f, 0.0f, -30.0f);
			transform.Position = glm::vec3(0.0f, 0.2f, -0.1f);
			tintedTextureTile.GetTransformHandle() = transform;

			tintedTextureTile.Emplace<CCamera>();
			m_Scene->SetPrimaryCameraEntity(tintedTextureTile);
		}

		Entity flatTile = enviroActor.CreateChildEntity();
		{
			auto& tile = flatTile.Emplace<CTile>().Tile;
			tile.Color = glm::vec4(0.1f, 0.1f, 1.0f, 1.0f);
			tile.TextureTilingFactor = 3;

			Transform transform;
			transform.Scale = glm::vec3(0.4f, 0.3f, 1.0f);
			transform.Rotation = glm::vec3(0.0f, 0.0f, 20.0f);
			flatTile.GetTransformHandle() = transform;
		}

		Actor childActor = enviroActor.CreateAttachedActor("TestChildActor");
		{
			auto& tile = childActor.Emplace<CTile>().Tile;
			tile.Color = glm::vec4(0.8f, 0.8f, 0.1f, 1.0f);
			tile.TextureTilingFactor = 2;

			Transform transform;
			transform.Position = glm::vec3(0.3f, -0.2f, 0.19f);
			transform.Scale = glm::vec3(0.2f, 0.4f, 1.0f);
			transform.Rotation = glm::vec3(0.0f, 0.0f, -40.0f);
			childActor.GetTransformHandle() = transform;
		}

		Entity childOfChildActor = childActor.CreateChildEntity();

		Entity colorSprite = enviroActor.CreateChildEntity();
		{
			auto& sprite = colorSprite.Emplace<CSprite>().Sprite;
			sprite.Color = glm::vec4(0.9f, 0.2f, 0.9f, 0.8f);

			Transform transform;
			transform.Position = glm::vec3(-0.1f, -0.1f, 0.1f);
			transform.Scale = glm::vec3(0.3f, 0.2f, 1.0f);
			colorSprite.GetTransformHandle() = transform;
		}

		Actor playerActor = m_Scene->GetGameplayWorld()->CreateActor("Player");
		{
			TextureLibrary::Add(Texture2D::Create("assets/textures/Texture_with_Transparency.png"));

			auto& sprite = playerActor.Emplace<CSprite>().Sprite;
			sprite.Texture = TextureLibrary::Get("Texture_with_Transparency");

			Transform transform;
			transform.Position = glm::vec3(0.0f, 0.0f, 0.2f);
			transform.Scale = glm::vec3(0.3f, 0.3f, 1.0f);
			playerActor.GetTransformHandle() = transform;

			auto tags = playerActor.GetTagsHandle();
			tags.Add(Tags::Player);
			playerActor.GetTagsHandle().SetLocal(tags);

			PlayerMovementBehavior* movement = playerActor.CreateBehavior<PlayerMovementBehavior>();
			movement->m_Player = Entity(playerActor);
			movement->m_Movement.Set(Entity(playerActor));

			playerActor.Emplace<CPlayerMovement>();
			ComponentTypesRegistry::s_Registry.RegisterDataComponent<CPlayerMovement>();
		}

		Entity testChild_1 = playerActor.CreateChildEntity("ChildEntity_1");
		{
			auto& sprite = testChild_1.Emplace<CSprite>().Sprite;
			sprite.Texture = TextureLibrary::Get("Texture_with_Transparency");
			sprite.Color = { 1.0f, 1.0f, 1.0f, 0.5f };

			Transform transform;
			transform.Position = glm::vec3(0.8f, 0.8f, 0.3f);
			transform.Rotation = glm::vec3(0.0f, 0.0f, 20.0f);
			transform.Scale = glm::vec3(0.5f, 0.5f, 1.0f);
			testChild_1.GetTransformHandle().SetLocal(transform);
		}

		Entity testCild_2 = playerActor.CreateChildEntity("ChildEntity_2");
		{
			auto& sprite = testCild_2.Emplace<CSprite>().Sprite;
			sprite.Texture = TextureLibrary::Get("Default_Texture");
			sprite.Color = { 1.0f, 1.0f, 1.0f, 0.5f };

			Transform transform;
			transform.Position = glm::vec3(-0.8f, -0.8f, 0.0f);
			transform.Rotation = glm::vec3(0.0f, 0.0f, -20.0f);
			transform.Scale = glm::vec3(0.7f, 0.4f, 1.0f);
			testCild_2.GetTransformHandle().SetLocal(transform);
		}
	}

	void EditorLayer::OnEvent(Ref<Events::Event> event)
	{
		FE_LOG_TRACE("{0}", event);

		m_CameraController->OnEvent(event);

		Events::EventDispacher dispacher(event);
		dispacher.Dispach<Events::KeyPressedEvent>(FE_BIND_EVENT_HANDLER(EditorLayer::OnKeyPressedEvent));
	}

	void EditorLayer::OnKeyPressedEvent(Ref<Events::KeyPressedEvent> event)
	{
	
	}

}