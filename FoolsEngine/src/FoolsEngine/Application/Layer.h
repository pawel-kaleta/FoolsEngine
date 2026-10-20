#pragma once

#include "FoolsEngine/Platform/Events/Event.h"
#include "FoolsEngine/Foundation/Utils/Core.h"
#include "FoolsEngine/Foundation/Debug/Profiler.h"
#include "FoolsEngine/Foundation/Context/Pile.h"
#include "FoolsEngine/Foundation/Containers/StringBuilder.h"

namespace fe
{
	class Layer
	{
	public:
		Layer(String name = String("Layer"_FE))
			: m_Name(name)
		{
			FE_PROFILER_FUNC();
		}
		virtual ~Layer() = default;

		virtual void OnAttach() { }
		virtual void OnDetach() { }
		virtual void OnUpdate() { }
		virtual void OnImGuiRender() { }
		virtual void OnEvent(Ref<Events::Event> event) { }

		const std::string& ToString() const
		{
			Pile p;
			return GetCString(m_Name , &p).Data;
		}

		String m_Name;
	};

}

