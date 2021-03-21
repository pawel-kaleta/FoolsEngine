#pragma once

#include "FoolsEngine/Core.h"
#include "FoolsEngine/Events/Events.h"

namespace fe {

	class Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

	protected:
		MainDispacher m_MainEventDispacher;
	};

	// To be defined in FoolsEngine application (game)
	Application* CreateApplication();
}