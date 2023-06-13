#pragma once

#include "FoolsEngine/Events/Event.h"
#include "FoolsEngine/Core/InputCodes.h"

/*
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
		class MouseMovedEvent : public Event
		{
		public:
			MouseMovedEvent(const float x, const float y)
				: m_MouseX(x), m_MouseY(y) {}

			float GetX() const { return m_MouseX; }
			float GetY() const { return m_MouseY; }

			std::string ToString() const override
			{
				std::stringstream ss;
				ss << "MouseMovedEvent: x=" << m_MouseX << ", y=" << m_MouseY;
				return ss.str();
			}

			EVENT_CLASS_TYPE(EventType::MouseMoved);
			EVENT_CLASS_CATEGORY(EventCategory::Input | EventCategory::Mouse);
		private:
			float m_MouseX, m_MouseY;
		};

		class MouseScrolledEvent : public Event
		{
		public:
			MouseScrolledEvent(const float x, const float y)
				: m_OffsetX(x), m_OffsetY(y) {}

			float GetOffsetX() const { return m_OffsetX; }
			float GetOffsetY() const { return m_OffsetY; }

			std::string ToString() const override
			{
				std::stringstream ss;
				ss << "MouseScrolledEvent: OffsetX=" << m_OffsetX << ", OffsetY=" << m_OffsetY;
				return ss.str();
			}

			EVENT_CLASS_TYPE(EventType::MouseScrolled);
			EVENT_CLASS_CATEGORY(EventCategory::Input | EventCategory::Mouse);
		private:
			float m_OffsetX, m_OffsetY;
		};

		class MouseButtonEvent : public Event
		{
		public:
			InputCodes::Mouse GetMouseButtonCode() const { return m_ButtonCode; }

			EVENT_CLASS_CATEGORY(EventCategory::Input | EventCategory::Mouse | EventCategory::MouseButton);
		protected:
			MouseButtonEvent(const InputCodes::Mouse buttonCode)
				: m_ButtonCode(buttonCode) {}

			InputCodes::Mouse m_ButtonCode;
		};

		class MouseButtonPressedEvent : public MouseButtonEvent
		{
		public:
			MouseButtonPressedEvent(const InputCodes::Mouse buttonCode)
				: MouseButtonEvent(buttonCode) {}

			std::string ToString() const override
			{
				std::stringstream ss;
				ss << "MouseButtonPressedEvent: " << m_ButtonCode;
				return ss.str();
			}

			EVENT_CLASS_TYPE(EventType::MouseButtonPressed);
		};

		class MouseButtonReleasedEvent : public MouseButtonEvent
		{
		public:
			MouseButtonReleasedEvent(const InputCodes::Mouse buttonCode)
				: MouseButtonEvent(buttonCode) {}

			std::string ToString() const override
			{
				std::stringstream ss;
				ss << "MouseButtonReleasedEvent: " << m_ButtonCode;
				return ss.str();
			}

			EVENT_CLASS_TYPE(EventType::MouseButtonReleased);
		};
	}
}