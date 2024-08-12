#pragma once

#include "FoolsEngine/Events/Event.h"
#include "FoolsEngine/Platform/InputCodes.h"

/*  cheat sheet

	enum class EventType
	{
		None = 0,
		WindowClose, WindowLostFocus, WindowFocus, WindowResize,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled,
		Custom
	};

	enum EventCategory
	{
		None        = 0,
		App         = BIT_FLAG(1),
		Input	    = BIT_FLAG(2),
		Keyboard    = BIT_FLAG(3),
		Mouse	    = BIT_FLAG(4),
		MouseButton = BIT_FLAG(5)
	};

		EVENT_CLASS_TYPE(EventType::type);
		EVENT_CLASS_CATEGORY(EventCategory::category);
*/

namespace fe
{
	namespace Events
	{
		class KeyEvent : public Event
		{
		public:
			InputCodes::Key GetKeyCode() const { return m_KeyCode; }

			EVENT_CLASS_CATEGORY(EventCategory::Input | EventCategory::Keyboard);
		protected:
			KeyEvent(const InputCodes::Key keyCode)
				: m_KeyCode(keyCode) {}
		
			InputCodes::Key m_KeyCode;
		};
	
		class KeyPressedEvent : public KeyEvent
		{
		public:
			KeyPressedEvent(const InputCodes::Key keyCode, const uint16_t RepeatCount)
				: KeyEvent(keyCode), m_RepeatCount(RepeatCount) {}

			uint16_t GetRepeatCount() const { return m_RepeatCount; }

			std::string ToString() const override
			{
				std::stringstream ss;
				ss << "KeyPressedEvent: " << m_KeyCode << " (" << m_RepeatCount << " repeats)";
				return ss.str();
			}

			EVENT_CLASS_TYPE(EventType::KeyPressed);
		private:
			uint16_t m_RepeatCount;
		};

		class KeyReleasedEvent : public KeyEvent
		{
		public:
			KeyReleasedEvent(const InputCodes::Key keyCode)
				: KeyEvent(keyCode) {}


			std::string ToString() const override
			{
				std::stringstream ss;
				ss << "KeyReleasedEvent: " << m_KeyCode;
				return ss.str();
			}

			EVENT_CLASS_TYPE(EventType::KeyReleased);
		};

		class KeyTypedEvent : public KeyEvent
		{
		public:
			KeyTypedEvent(const InputCodes::Key keyCode)
				: KeyEvent(keyCode) {}

			std::string ToString() const override
			{
				std::stringstream ss;
				ss << "KeyTypedEvent: " << m_KeyCode;
				return ss.str();
			}

			EVENT_CLASS_TYPE(EventType::KeyTyped);
		};
	}
}