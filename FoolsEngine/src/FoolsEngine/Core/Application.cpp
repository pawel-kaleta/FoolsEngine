#include "FE_pch.h"
#include "FoolsEngine/Core/Application.h"
#include "FoolsEngine/Debug/Profiler.h"
#include "FoolsEngine/Platform/Win10/Win10Window.h"

#include "GLFW/glfw3.h"

namespace fe {

	Application::Application()
	{
		FE_PROFILER_FUNC();

		m_Window = std::unique_ptr<Window>(Window::Create());
		m_MainEventDispacher = std::make_unique<MainDispacher>();

		m_Window->SetEventCallback(std::bind(&MainDispacher::ReceiveEvent, m_MainEventDispacher.get(), std::placeholders::_1));
		m_MainEventDispacher->AddSubscription(std::bind(&Application::OnEvent, this, std::placeholders::_1));
	}

	Application::~Application()
	{
		FE_PROFILER_FUNC();
	}

	void Application::OnEvent(std::shared_ptr<Event> event)
	{
		//event->Handled = true;
	}

	void Application::Run()
	{
		FE_PROFILER_FUNC();

		while (m_Running)
		{
			m_Window->OnUpdate();
			m_MainEventDispacher->DispachEvents();
		}
			
	}
}