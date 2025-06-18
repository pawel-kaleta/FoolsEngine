#include "EditorApp.h"

#include <EntryPoint.h>

#include "EditorLayer.h"

namespace fe
{
	EditorApp::EditorApp(const ApplicationSpecification& spec)
		: Application(spec)
	{
		
	}

	void EditorApp::ClientAppStartup()
	{
		m_EditorLayer = CreateRef<EditorLayer>();
		PushOuterLayer(m_EditorLayer);
	}

	void EditorApp::ClientAppShutdown()
	{
		PopOuterLayer(m_EditorLayer);
		m_EditorLayer.reset();
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