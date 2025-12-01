#include "EditorApp.h"

#include <EntryPoint.h>

#include "EditorLayer.h"
#include "Asset\FileHandler.h"
#include "Asset\CreateAsset.h"

namespace fe
{
	EditorApp::EditorApp(const ApplicationSpecification& spec)
		: Application(spec)
	{ }

	void EditorApp::ClientAppStartup()
	{
		AssetImportModal::Init();
		m_EditorLayer = CreateRef<EditorLayer>();
	}

	void EditorApp::ClientAppShutdown()
	{
		if (m_LayerAttached)
			PopOuterLayer(m_EditorLayer);
		m_EditorLayer.reset();
	}

	bool EditorApp::ClientAppProjectInit()
	{
		ImGuiStyle& style = ImGui::GetStyle();

		static bool s_load_not_create = true;
		static char s_load_path_buffer[128] = "..\\Sandbox\\SandboxProject.feproj";
		static char s_create_path_buffer[128] = "";

		if (s_load_not_create)
		{
			ImVec2 center = ImGui::GetMainViewport()->GetCenter();
			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

			ImGuiWindowFlags_ window_flags = (ImGuiWindowFlags_)(
				ImGuiWindowFlags_NoCollapse |
				ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_NoResize);
			if (ImGui::Begin("New/Open Project", nullptr, window_flags))
			{
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, style.DisabledAlpha);
				if (ImGui::Button("New Project"))
					s_load_not_create = false;
				ImGui::PopStyleVar();

				ImGui::SameLine();
				ImGui::Text("<->");
				ImGui::SameLine();

				ImGui::Button("Open Project");

				ImGui::Text("Path");
				ImGui::SameLine();

				ImGui::InputText("##filepath", s_load_path_buffer, 128);
				ImGui::SameLine();

				std::filesystem::path filepath;
				if (ImGui::Button("Browse"))
				{
					filepath = FileDialogs::OpenFile("FoolsEngine Project (*.feproj)\0*.feproj\0");
					if (!filepath.empty())
						strncpy_s(s_load_path_buffer, filepath.string().c_str(), sizeof(s_load_path_buffer));
				}

				if (ImGui::Button("Load"))
				{
					filepath = s_load_path_buffer;
					Application::ProjectLoad(filepath);
					PushOuterLayer(m_EditorLayer);
					ImGui::End();
					return false;
				}

				ImGui::SameLine();
				if (ImGui::Button("Exit"))
				{
					Application::Close();
				}

				ImGui::End();
			}
			else
			{
				FE_LOG_CORE_FATAL("Failed to display launch modal window");
				Application::Close();
			}
		}
		else
		{
			if (ImGui::Begin("New/Open Project"))
			{
				ImGui::Button("New Project");

				ImGui::SameLine();
				ImGui::Text("<->");
				ImGui::SameLine();

				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, style.DisabledAlpha);
				if (ImGui::Button("Open Project"))
					s_load_not_create = true;
				ImGui::PopStyleVar();

				ImGui::Text("Path");
				ImGui::SameLine();

				ImGui::InputText("##filepath", s_create_path_buffer, 128);
				ImGui::SameLine();

				std::filesystem::path filepath;
				if (ImGui::Button("Browse"))
				{
					filepath = FileDialogs::SaveFile("", "FoolsEngine Project (*.feproj)\0*.feproj\0");
					if (!filepath.empty())
						strncpy_s(s_create_path_buffer, filepath.string().c_str(), sizeof(s_create_path_buffer));
				}

				if (ImGui::Button("Create"))
				{
					filepath = s_create_path_buffer;
					Application::ProjectNew(filepath);
					PushOuterLayer(m_EditorLayer);
					ImGui::End();
					return false;
				}

				ImGui::SameLine();
				if (ImGui::Button("Exit"))
				{
					Application::Close();
				}

				ImGui::End();
			}
			else
			{
				FE_LOG_CORE_FATAL("Failed to display launch modal window");
				Application::Close();
			}
		}
		
		return true;
	}

	Application* CreateApplication(const ApplicationCommandLineArgs& args)
	{
		ApplicationSpecification app_spec;

		app_spec.CommandLineArgs = args;
		app_spec.Name = "FoolsTools";
		app_spec.WindowAttributes = { "FoolsTools", 1900, 1000, GAPIType::OpenGL };

		return new EditorApp(app_spec);
	}
}