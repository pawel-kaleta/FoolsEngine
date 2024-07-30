#include "EditorApp.h"

#include <EntryPoint.h>

#include "EditorAssetManager.h"
#include "EditorLayer.h"

namespace fe
{
	EditorApp::EditorApp(const std::string& name)
		: Application(name, WindowAttributes{ name, 1920, 1080, GDIType::OpenGL })
	{
		
	}

	void EditorApp::ClientAppStartup()
	{
		m_EditorAssetManager = Scope<EditorAssetManager>(new EditorAssetManager());
		m_EditorLayer = CreateRef<EditorLayer>();
		PushOuterLayer(m_EditorLayer);
	}

	void EditorApp::ClientAppShutdown()
	{
		PopOuterLayer(m_EditorLayer);
		m_EditorLayer.reset();
		m_EditorAssetManager.reset();
	}

	Application* CreateApplication()
	{
		return new EditorApp("FoolsTools - FoolsEngine Editor");
	}
}