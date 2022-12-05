#pragma once

#include "FoolsEngine/Core/Window.h"
#include "FoolsEngine\Renderer\RenderingContext.h"

#include <GLFW/glfw3.h>

namespace fe
{
	class Win10Window : public Window
	{
	public:
		Win10Window(const WindowAttributes& attr);
		~Win10Window();

		void OnUpdate() override;

		unsigned int GetWidth() const override { return m_Data.Width; };
		unsigned int GetHeight() const override { return m_Data.Height; };
		std::string GetTitle() const override { return m_Data.Title; };

		void SetEventCallback(const std::function<void(std::shared_ptr<Event>)>& callback) override { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override { return m_Data.VSync; };

		void* GetNativeWindow() const override { return m_Window; };

	private:
		GLFWwindow* m_Window;
		RenderingContext* m_RenderingContext;

		struct WinData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			std::function<void(std::shared_ptr<Event>)> EventCallback;
		};

		WinData m_Data;

		void Init(const WindowAttributes& attr);
		void SetGLFWEventsCallbacks();
		void ShutDown();

		static void GLFWErrorCallback(int error, const char* msg);
	};
}
