#include "EditorApp.h"

#include <EntryPoint.h>

#include "EditorLayer.h"

namespace fe
{
	EditorApp::EditorApp(const ApplicationSpecification& spec)
		: Application(spec)
	{ }

	void EditorApp::ClientAppStartup()
	{
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
		
		if (!ImGui::Begin("New/Open Project")) ImGui::End();
		
		static bool load_not_create = true;
		if (load_not_create)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, style.DisabledAlpha);
			if (ImGui::Button("New Project"))
				load_not_create = false;
			ImGui::PopStyleVar();
		}
		else
		{
			ImGui::Button("New Project");
		}
		
		ImGui::SameLine();
		ImGui::Text("<->");
		ImGui::SameLine();

		if (load_not_create)
		{
			ImGui::Button("Open Project");
		}
		else
		{
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, style.DisabledAlpha);
			if (ImGui::Button("Open Project"))
				load_not_create = true;
			ImGui::PopStyleVar();
		}

		bool project_loaded_or_created = false;
		std::filesystem::path filepath;

		ImGui::Text("Path");
		ImGui::SameLine();
		if (load_not_create)
		{
			static char load_path_buffer[128] = ""; ImGui::InputText("##filepath", load_path_buffer, 128);
			ImGui::SameLine();
			if (ImGui::Button("Browse"))
			{
				filepath = FileDialogs::OpenFile("FoolsEngine Project (*.feproj)\0*.feproj\0");
				if (!filepath.empty())
					strncpy_s(load_path_buffer, filepath.string().c_str(), sizeof(load_path_buffer));
			}

			if (ImGui::Button("Load"))
			{
				filepath = load_path_buffer;
				Application::ProjectLoad(filepath);
				project_loaded_or_created = true;
			}
		}
		else
		{
			static char create_path_buffer[128] = ""; ImGui::InputText("##filepath", create_path_buffer, 128);
			ImGui::SameLine();
			if (ImGui::Button("Browse"))
			{
				filepath = FileDialogs::SaveFile("", "FoolsEngine Project (*.feproj)\0*.feproj\0");
				if (!filepath.empty())
					strncpy_s(create_path_buffer, filepath.string().c_str(), sizeof(create_path_buffer));
			}

			if (ImGui::Button("Create"))
			{
				filepath = create_path_buffer;
				Application::ProjectNew(filepath);
				project_loaded_or_created = true;
			}
		}

		ImGui::End(); // window
		
		if (project_loaded_or_created)
		{
			PushOuterLayer(m_EditorLayer);
			return false;
		}
		return true;
	}

	Application* CreateApplication(const ApplicationCommandLineArgs& args)
	{
		ApplicationSpecification app_spec;

		app_spec.CommandLineArgs = args;
		app_spec.Name = "FoolsTools";
		app_spec.WindowAttributes = { "FoolsTools", 1920, 1080, GDIType::OpenGL };

		return new EditorApp(app_spec);
	}
}