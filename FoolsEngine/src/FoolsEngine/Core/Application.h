#pragma once

#include "FoolsEngine/Core/Core.h"
#include "FoolsEngine/Events/Events.h"
#include "FoolsEngine/Core/Window.h"

namespace fe {

	class Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

	private:
		std::unique_ptr<MainDispacher> m_MainEventDispacher;
		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
	};

	// To be defined in FoolsEngine application (game)
	Application* CreateApplication();
}