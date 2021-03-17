#pragma once

#include "FoolsEngine/Core.h"
#include "FoolsEngine/Debug/Log.h"

#include <functional>
#include <vector>
#include <memory>
/*
namespace fe
{
	// Events are blocking (for now) - when created, must be immediately handled

	int lastNewEventClassID = 0;
	int getNewEventClassID() { return ++lastNewEventClassID; }

	enum class EventType
	{
		None = 0,
		WindowClose, WindowLostFocus, WindowFocus, WindowMoved, WindowResize,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled,
		Custom
	};
	
	// macro for easier implementations of events
	#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::type; } \
	                               virtual EventType GetEventType() const override { return GetStaticType(); } \
	                               virtual const char* GetName() const override { return #type; }
	// GetStaticType() - do not need an instance to read a type
	// GetEventType()  - base Event class forces this implementation, also need a way to read type from unknown instance							   
	// GetName()       - base Event class forces this implementation, also need a way to read type from unknown instance
	

	enum EventCategory
	{
		None        = 0,
		EngineSys   = BIT_FLAG(1),
		Window      = BIT_FLAG(2),
		RunLoop     = BIT_FLAG(3),
		Input	    = BIT_FLAG(4),
		Keyboard    = BIT_FLAG(5),
		Mouse	    = BIT_FLAG(6),
		MouseButton = BIT_FLAG(7),
		Custom      = BIT_FLAG(8)
	};

	// macro for easier implementations of events
	#define EVENT_CLASS_CATEGORY(category) static int GetStaticCategoryFlags() { return EventCategory::category; } \
	                                       virtual int GetCategoryFlags() const override { return EventCategory::category; }
	// GetStaticCategoryFlags() - do not need an instance to read category flags
	// GetCategoryFlags()       - base Event class forces this implementation - need a way to read category flags from unknown instance


	class FE_API Event
	{
	public:
		virtual ~Event() = default;

		bool Handled = false;

		virtual EventType GetEventType() const { return m_eventType; };
		const static EventType m_eventType = EventType::None;
		virtual const char* GetName() const { return "None"; }
		virtual int GetCategoryFlags() const { return m_categoryFlags; }
		const static int GetStaticCategoryFlags() { return m_categoryFlags; }
		virtual std::string ToString() const { return GetName(); }
		bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category; // bitwise AND (not "adress-of")
		}
	protected:
		Event() {};
		static int m_categoryFlags;
	};
	int Event::m_categoryFlags = EventCategory::None;

	class FE_API CustomEvent : Event
	{
	public:
		CustomEvent() {};
		virtual int GetCategoryFlags() const override { return m_categoryFlags; }
		const static int m_categoryFlags = Event::m_categoryFlags + EventCategory::Custom;
		static int getStaticID() { return m_id; }
		virtual int getID() const { return m_id; }
	private:
		static int m_id;
	};
	int CustomEvent::m_id = getNewEventClassID();

	/*
	CUSTOM EVENT CLASS TEMPLATE
	class FE_API NewEventClass : BaseEventClass
	{
	public:
		NewEventClass() {};
		virtual int GetCategoryFlags() const override { return m_categoryFlags; }
		const static int m_categoryFlags = Event::m_categoryFlags; //add other categories using | operator
		static int getStaticID() { return m_id; }
		virtual int getID() const override { return m_id; }
	private:
		static int m_id;
	};
	int NewEventClass::m_id = getNewEventClassID(); // do not ever again use m_id directly, use getID() and getStaticID() instead
	*/
/*
	class FE_API EventSubscryption
	{
	public:
		EventSubscryption(std::function<void(std::shared_ptr<Event>)> handler, EventType type)
			: m_handler(handler), m_condition(type) {}

		std::function<void(std::shared_ptr<Event>)> m_handler;
		EventType m_condition;
	};

	class FE_API EventDispacher
	{
	public:
		void AddSubscription(std::shared_ptr<EventSubscryption> subscription);
		void RemoveSubscription(std::shared_ptr<EventSubscryption> subscription);
		virtual void ReceiveEvent(std::shared_ptr<Event> event) const = 0;

		~EventDispacher()
		{
			m_subscryptions->clear();
			delete m_subscryptions;
		}

	protected:
		EventDispacher()
			: m_subscryptions(new std::vector<std::shared_ptr<EventSubscryption>>)
		{ }

		std::vector<std::shared_ptr<EventSubscryption>>* m_subscryptions;
	};

	class FE_API EventDispacherBuffering : EventDispacher
	{
	public:
		void ReceiveEvent(std::shared_ptr<Event> event) const override;
		void Dispach();

		EventDispacherBuffering()
			: m_eventsQueue(new std::vector<std::shared_ptr<Event>>)
		{ }

		~EventDispacherBuffering()
		{
			m_eventsQueue->clear();
			delete m_eventsQueue;
		}

	protected:
		std::vector<std::shared_ptr<Event>>* m_eventsQueue;
	};

	class FE_API EventDispacherBlocking : EventDispacher
	{
	public:
		void ReceiveEvent(std::shared_ptr<Event> event) const override;

		EventDispacherBlocking()
			: m_subscryptions(new std::vector<std::shared_ptr<EventSubscryption>>)
		{ }

		~EventDispacherBlocking()
		{
			m_subscryptions->clear();
			delete m_subscryptions;
		}

	protected:
		std::vector<std::shared_ptr<EventSubscryption>>* m_subscryptions;
	};
}*/