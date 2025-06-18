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
		if (ImGui::Begin("New/Open Project"))
		{

			ImGui::End();
		}
		return true;

		{
			PushOuterLayer(m_EditorLayer);
			return false;
		}
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