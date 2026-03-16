#pragma once

#include "FoolsEngine/Foundation/Utils/Core.h"

#include "FoolsEngine/Platform/Events/Event.h"

#include "FoolsEngine/Application/LayerStack.h"

namespace fe
{
	class MainEventDispacher
	{
	public:
		MainEventDispacher() { FE_PROFILER_FUNC(); }

		void ReceiveEvent(Ref<Events::Event> event);
		void DispachEvents(LayerStack& layerStack);
	private:
		// double buffering to handle events creation during dispaching
		std::vector<Ref<Events::Event>> m_InputBuffer;
		std::vector<Ref<Events::Event>> m_OutputBuffer;
	};
}
