#pragma once
#include "FE_pch.h"
#include "FoolsEngine/Events/Event.h"

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
	class WindowCloseEvent : public Event
	{
	public:
		EVENT_CLASS_TYPE(EventType::WindowClose);
		EVENT_CLASS_CATEGORY(EventCategory::App);
	};

	class WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(unsigned int width, unsigned int height)
			: m_Width(width), m_Height(height) {}

		unsigned int GetWidth() const {	return m_Width;	}
		unsigned int GetHeight() const { return m_Height;  }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
			return ss.str();
		}

		EVENT_CLASS_TYPE(EventType::WindowResize);
		EVENT_CLASS_CATEGORY(EventCategory::App);

	private:
		unsigned int m_Width, m_Height;
	};

	class WindowLostFocusEvent : public Event
	{
	public:
		EVENT_CLASS_TYPE(EventType::WindowLostFocus);
		EVENT_CLASS_CATEGORY(EventCategory::App);
	};

	class WindowGainedFocusEvent : public Event
	{
	public:
		EVENT_CLASS_TYPE(EventType::WindowGainedFocus);
		EVENT_CLASS_CATEGORY(EventCategory::App);
	};

	class AppTickEvent : public Event
	{
	public:
		EVENT_CLASS_TYPE(EventType::AppTick);
		EVENT_CLASS_CATEGORY(EventCategory::App);
	};

	class AppUpdateEvent : public Event
	{
	public:
		EVENT_CLASS_TYPE(EventType::AppUpdate);
		EVENT_CLASS_CATEGORY(EventCategory::App);
	};

	class AppRenderEvent : public Event
	{
	public:
		EVENT_CLASS_TYPE(EventType::AppRender);
		EVENT_CLASS_CATEGORY(EventCategory::App);
	};
}