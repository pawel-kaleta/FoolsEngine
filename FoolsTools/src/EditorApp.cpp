#include "EditorApp.h"

#include <EntryPoint.h>

#include "EditorAssetManager.h"
#include "EditorLayer.h"

namespace fe
{
	EditorApp::EditorApp(const std::string& name)
		: Application(name, WindowAttributes{ name, 1920, 1080 })
	{
		m_EditorAssetManager = Scope<EditorAssetManager>(new EditorAssetManager());
		PushOuterLayer(CreateScope<EditorLayer>());
	}

	Application* CreateApplication()
	{
		return new EditorApp("FoolsTools - FoolsEngine Editor");
	}
}