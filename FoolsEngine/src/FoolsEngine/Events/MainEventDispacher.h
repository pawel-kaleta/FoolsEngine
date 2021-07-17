#pragma once

#include "FoolsEngine/Core/Core.h"
#include "FoolsEngine/Events/Event.h"
#include "FoolsEngine/Core/LayerStack.h"

namespace fe
{
	class MainEventDispacher
	{
	public:
		MainEventDispacher() { FE_PROFILER_FUNC(); }

		void ReceiveEvent(std::shared_ptr<Event> event);
		void DispachEvents(LayerStack& layerStack);
	private:
		std::vector<std::shared_ptr<Event>> m_eventsQueue;
	};
}
