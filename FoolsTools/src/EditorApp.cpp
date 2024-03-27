#include <FoolsEngine.h>
#include <EntryPoint.h>

#include "EditorLayer.h"

namespace fe
{
	class EditorApp : public Application
	{
	public:
		EditorApp(const std::string& name)
			: Application(name, WindowAttributes{name, 1920, 1080})
		{
			PushOuterLayer(CreateScope<EditorLayer>());
		}

		~EditorApp()
		{

		}
	};

	Application* CreateApplication()
	{
		return new EditorApp("FoolsTools - FoolsEngine Editor");
	}
}