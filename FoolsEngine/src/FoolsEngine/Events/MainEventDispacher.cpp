#include "FE_pch.h"

#include "FoolsEngine\Events\MainEventDispacher.h"

namespace fe
{
	void MainEventDispacher::ReceiveEvent(std::shared_ptr<Event> event)
	{
		FE_PROFILER_FUNC();

		FE_LOG_CORE_TRACE("NEW EVENT: {0}", event->ToString());
		m_eventsQueue.push_back(event);
		FE_LOG_CORE_TRACE("SAVED EVENT: {0}", m_eventsQueue.back()->ToString());
	}

	void MainEventDispacher::DispachEvents(LayerStack& layerStack)
	{
		FE_PROFILER_FUNC();

		if (m_eventsQueue.empty())
			return;

		for (auto event_it = m_eventsQueue.begin(); event_it != m_eventsQueue.end(); event_it++) // auto = std::vector< std::shared_ptr< Event > >::iterator
		{
			for (auto layer_it = layerStack.begin(); layer_it != layerStack.end(); layer_it++) // auto = std::vector< std::shared_ptr< Layer > >::iterator
			{
				(*layer_it)->OnEvent(*event_it);
				if ((*event_it)->Handled)
					break;
			}
			if (!((*event_it)->Handled))
			{
				FE_LOG_CORE_WARN("Unhandled event: {0}", (*event_it)->ToString());
			}
		}
		m_eventsQueue.clear();
		FE_LOG_CORE_DEBUG("Events dispached!");
	}
}

