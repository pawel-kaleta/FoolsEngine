#include "FE_pch.h"
#include "FoolsEngine/Core/Application.h"
#include "FoolsEngine/Debug/Profiler.h"
#include "FoolsEngine/Platform/Win10/Win10Window.h"

#include <glad/glad.h>

namespace fe {

	Application::Application()
	{
		FE_PROFILER_FUNC();

		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(std::bind(&MainEventDispacher::ReceiveEvent, & m_MainEventDispacher, std::placeholders::_1));

		m_AppLayer = std::make_shared<ApplicationLayer>(std::bind(&Application::OnEvent, this, std::placeholders::_1));
		m_LayerStack.PushOuterLayer(m_AppLayer);
	}

	Application::~Application()
	{
		FE_PROFILER_FUNC();
	}

	void Application::OnEvent(std::shared_ptr<Event> event)
	{
		FE_PROFILER_FUNC();

		if (event->GetEventType() == EventType::WindowClose)
		{
			OnWindowCloseEvent(event);
		}
	}

	void Application::OnWindowCloseEvent(std::shared_ptr<Event> event)
	{
		FE_PROFILER_FUNC();

		m_Running = false;
		event->Handled = true;
		FE_LOG_CORE_INFO("Window Close Event");
	}


	void Application::Run()
	{
		FE_PROFILER_FUNC();

		while (m_Running)
		{
			m_Window->OnUpdate();

			UpdateLayers();

			m_MainEventDispacher.DispachEvents(m_LayerStack);
		}
			
	}

	void Application::UpdateLayers()
	{
		for (auto layer_it = m_LayerStack.begin(); layer_it != m_LayerStack.end(); layer_it++) // auto = std::vector< std::shared_ptr< Layer > >::iterator
		{
			(*layer_it)->OnUpdate();
		}
	}

	void MainEventDispacher::ReceiveEvent(std::shared_ptr<Event> event)
	{
		FE_PROFILER_FUNC();

		FE_LOG_CORE_TRACE("NEW EVENT: {0}", event->ToString());
		m_eventsQueue.push_back(event);
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
				//(*layer_it).operator->()->OnEvent(*event_it);
				(*layer_it)->OnEvent(*event_it);
				if ((*event_it)->Handled)
					break;
			}
			if (!((*event_it)->Handled))
			{
				//FE_LOG_CORE_WARN("Unhandled event: {0}", (*event_it)->ToString());
			}
		}

		m_eventsQueue.clear();

		FE_CORE_ASSERT(m_eventsQueue.size() == 0, "Events buffer not cleared!");
		FE_LOG_CORE_DEBUG("Events dispached!");
	}
}