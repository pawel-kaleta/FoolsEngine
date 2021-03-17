#include "FoolsEngine/Events/Events.h"
/*
namespace fe
{
	void EventDispacher::AddSubscription(std::shared_ptr<EventSubscryption> subscription)
	{
		m_subscryptions->push_back(subscription);
	}

	void EventDispacher::RemoveSubscription(std::shared_ptr<EventSubscryption>)
	{
		FE_LOG_CORE_FATAL("Feature not implementet yet: EventDispacher::RemoveSubscription()");
	}

	void EventDispacherBlocking::ReceiveEvent(std::shared_ptr<Event> event) const
	{
		for (std::vector<std::shared_ptr<EventSubscryption>>::iterator it = m_subscryptions->begin(); it != m_subscryptions->end(); ++it)
		{
			if (it->get()->m_condition == event->GetEventType())
			{
				it->get()->m_handler(event);
				if (event->Handled)
					break;
			}
		}
	}

	void EventDispacherBuffering::ReceiveEvent(std::shared_ptr<Event> event) const
	{
		m_eventsQueue->push_back(event);
	}

	void EventDispacherBuffering::Dispach()
	{
		while (!m_eventsQueue->empty())
		{
			std::shared_ptr<Event> eventToDispach = m_eventsQueue->back();
			m_eventsQueue->pop_back();

			for (std::vector<std::shared_ptr<EventSubscryption>>::iterator it = m_subscryptions->begin(); it != m_subscryptions->end(); ++it)
			{
				if (it->get()->m_condition == eventToDispach->GetEventType())
				{
					it->get()->m_handler(eventToDispach);
					if (eventToDispach->Handled)
						break;
				}
			}
		}
	}
}*/