#pragma once

#include "FoolsEngine/Core.h"

namespace fe {

	class Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	// To be defined in FoolsEngine application (game)
	Application* CreateApplication();
}