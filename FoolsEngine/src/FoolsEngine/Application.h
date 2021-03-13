#pragma once

#include "Core.h"

namespace fe {

	class FE_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	// To be defined in FoolsEngine application (game)
	Application* CreateApplication();
}