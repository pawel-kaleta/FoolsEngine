#include "FE_pch.h"
#include "FoolsEngine/Events/Events.h"

namespace fe
{
	void EventDispacher::AddSubscription(std::function<void(std::shared_ptr<Event>)> subscription)
	{
		m_subscryptions.push_back(subscription);
	}

	void EventDispacher::RemoveSubscription(std::function<void(std::shared_ptr<Event>)> subscription)
	{
		FE_LOG_CORE_ERROR("Feature not implementet yet: EventDispacher::RemoveSubscription()");
	}

	bool EventDispacher::IsSubscription(std::function<void(std::shared_ptr<Event>)> subscription)
	{
		FE_LOG_CORE_ERROR("Feature not implementet yet: EventDispacher::IsSubscription()");
		return false;
	}

	void MainDispacher::ReceiveEvent(std::shared_ptr<Event> event)
	{
		m_eventsQueue.push_back(event);
	}

	void LayerDispacher::ReceiveEvent(std::shared_ptr<Event> event)
	{ 
		DispachEvent(event);
	}

	void MainDispacher::DispachEvents()
	{
		if (m_eventsQueue.empty() || m_subscryptions.empty())
			return;

		for (auto event_it = m_eventsQueue.begin(); event_it != m_eventsQueue.end(); event_it++) // auto = std::vector< std::shared_ptr< Event > >::iterator
		{
			for (auto sub_it = m_subscryptions.begin(); sub_it != m_subscryptions.end(); ++sub_it) // auto = std::vector<std::function<void(std::shared_ptr<Event>)>>::iterator
			{
				(*sub_it).operator()(*event_it);
				if ((*event_it)->Handled)
				{
					m_eventsQueue.erase(event_it);
					event_it--;
					break;
				}
			}
		}
		
	}
}