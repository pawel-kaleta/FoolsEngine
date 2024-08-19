#include "FE_pch.h"
#include "FoolsEngine\Events\MainEventDispacher.h"

namespace fe
{
	void MainEventDispacher::ReceiveEvent(Ref<Events::Event> event)
	{
		FE_PROFILER_FUNC();

		FE_LOG_CORE_TRACE("NEW EVENT: {0}", event->ToString());
		m_InputBuffer.push_back(event);
	}

	void MainEventDispacher::DispachEvents(LayerStack& layerStack)
	{
		FE_PROFILER_FUNC();
		FE_LOG_CORE_TRACE("Dispaching events");

		m_OutputBuffer.swap(m_InputBuffer);

		if (m_OutputBuffer.empty())
			return;

		for (auto event_it = m_OutputBuffer.begin(); event_it != m_OutputBuffer.end(); event_it++) // auto = std::vector< Ref< Event > >::iterator
		{
			for (auto layer_it = layerStack.begin(); layer_it != layerStack.end(); layer_it++) // auto = std::vector< Ref< Layer > >::iterator
			{
				(*layer_it)->OnEvent(*event_it);
				if ((*event_it)->Owned)
					break;
			}
		}
		m_OutputBuffer.clear();
	}
}

