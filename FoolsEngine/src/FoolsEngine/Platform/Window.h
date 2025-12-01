#pragma once

#include "FoolsEngine\Renderer\1 - Description\GAPIType.h"

namespace fe
{
	namespace Events
	{
		class Event;
	}

	struct WindowAttributes
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;
		GAPIType GAPI;

		WindowAttributes(const std::string& title = "FoolsEngine",
		                 uint32_t width = 1600,
		                 uint32_t height = 900,
			             GAPIType GAPI = GAPIType::OpenGL)
			: Title(title), Width(width), Height(height), GAPI(GAPI)
		{ }
	};

	// TO DO: get rid of virtualisation and make a compile time decision about choosing implementation with a macro

	class Window
	{
	public:
		virtual ~Window() = default;

		virtual void CreateRenderingContext(GAPIType GAPI) = 0;
		        void CreateRenderingContext() { CreateRenderingContext(m_GAPI); }
		virtual void MakeRenderingContextCurrent(GAPIType GAPI) = 0;

		virtual void OnUpdate() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual std::string GetTitle() const = 0;
		GAPIType GetGAPIType() { return m_GAPI; }

		virtual void SetEventCallback(const std::function<void(std::shared_ptr<Events::Event>)>& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;

		static Window* Create(const WindowAttributes& attr = WindowAttributes());

	protected:
		GAPIType m_GAPI;
	};
}


