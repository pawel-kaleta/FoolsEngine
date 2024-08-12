#pragma once

namespace fe
{
	namespace Events
	{
		class Event;
	}

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
		virtual void OnImGuiRender() { }
		virtual void OnEvent(Ref<Events::Event> event) { }

		const std::string& GetName() const { return m_Name; }
		const std::string& ToString() const { return m_Name; }
	protected:
		std::string m_Name;
	};

}

