#pragma once
#include "FE_pch.h"
#include "FoolsEngine/Core/Core.h"
#include "FoolsEngine/Debug/Log.h"

namespace fe
{
	enum class EventType
	{
		None = 0,
		WindowClose, WindowLostFocus, WindowGainedFocus, WindowResize,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled,
		Custom
	};
	
	// macro for easier implementations of events
	#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return type; } \
	                               virtual EventType GetEventType() const override { return GetStaticType(); } \
	                               virtual const char* GetName() const override { return #type; }
	// GetStaticType() - do not need an instance to read a type
	// GetEventType()  - base Event class forces this implementation, also need a way to read type from unknown instance							   
	// GetName()       - base Event class forces this implementation, also need a way to read type from unknown instance
	

	enum EventCategory
	{
		None        = 0,
		App         = BIT_FLAG(1),
		Input	    = BIT_FLAG(2),
		Keyboard    = BIT_FLAG(3),
		Mouse	    = BIT_FLAG(4),
		MouseButton = BIT_FLAG(5)
	};

	// macro for easier implementations of events
	#define EVENT_CLASS_CATEGORY(category) static int GetStaticCategoryFlags() { return category; } \
	                                       virtual int GetCategoryFlags() const override { return category; }
	// GetStaticCategoryFlags() - do not need an instance to read category flags
	// GetCategoryFlags()       - base Event class forces this implementation - need a way to read category flags from unknown instance


	class Event
	{
	public:
		virtual ~Event() = default;

		bool Handled = false;

		virtual EventType GetEventType() const { return GetStaticEventType(); }
		static EventType GetStaticEventType() { return EventType::None; }

		virtual int GetCategoryFlags() const { return GetStaticCategoryFlags(); }
		static int GetStaticCategoryFlags() { return EventCategory::None; }

		virtual const char* GetName() const { return "Base Event Class"; }
		virtual std::string ToString() const { return GetName(); }

		bool IsInCategory(EventCategory category)
		{
			return GetStaticCategoryFlags() & category; // bitwise AND (not a reference)
		}
	};
}

#define FE_NEW_EVENT(callback, localName, type, ...) std::shared_ptr<type> localName = std::make_shared<type>(__VA_ARGS__);\
                                                     callback(localName);