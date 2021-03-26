#pragma once

#include "FE_pch.h"

#include "FoolsEngine/Events/Event.h"

namespace fe
{
	class Layer
	{
	public:
		Layer(const std::string& name = "Layer")
			: m_Name(name)
		{
			FE_PROFILER_FUNC();
		}
		virtual ~Layer() = default;

		virtual void OnAttach() { }
		virtual void OnDetach() { }
		virtual void OnUpdate() { }
		virtual void OnEvent(std::shared_ptr<Event> event) { }

		const std::string& GetName() const { return m_Name; }
		const std::string& ToString() const { return m_Name; }
	protected:
		std::string m_Name;
	};

}

