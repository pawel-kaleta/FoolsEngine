#include <FoolsEngine.h>
#include <EntryPoint.h>

#include "EditorLayer.h"

namespace fe
{
	class EditorApp : public Application
	{
	public:
		EditorApp()
		{
			PushInnerLayer(CreateScope<EditorLayer>());
		}

		~EditorApp()
		{

		}

	};

	Application* CreateApplication()
	{
		return new EditorApp();
	}
}