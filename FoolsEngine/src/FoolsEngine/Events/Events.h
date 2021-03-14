#pragma once

#include "FoolsEngine/Core.h"
#include "FoolsEngine/Log.h"

#include <functional>
#include <vector>
#include <memory>

namespace fe
{
	// Events are blocking (for now) - when created, must be immediately handled

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
	#define EVENT_CLASS_CATEGORY(category) static int GetStaticCategoryFlags() { return category; } \
	                                       virtual int GetCategoryFlags() const override { return #category; }
	// GetStaticCategoryFlags() - base Event class forces this implementation - need a way to read category flags from unknown instance
	// GetCategoryFlags()       - do not need an instance to read category flags


	class FE_API Event
	{
	public:
		virtual ~Event() = default;

		bool Handled = false;

		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }

		bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category; // bitwise AND (not "adress-of")
		}

	private:
		Event();
	};

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
		void ReceiveEvent(std::shared_ptr<Event> event) const override;

		EventDispacherBlocking()
			: m_subscryptions(new std::vector<std::shared_ptr<EventSubscryption>>)
		{ }

		~EventDispacherBlocking()
		{
			m_subscryptions->clear();
			delete m_subscryptions;
		}

	private:
		std::vector<std::shared_ptr<EventSubscryption>>* m_subscryptions;
	};
}