#pragma once

#include "FoolsEngine\Core\Window.h"
#include "FoolsEngine\Renderer\2 - GDIAbstraction\RenderingContext.h"

#include <GLFW/glfw3.h>

namespace fe
{
	class Win32Window : public Window
	{
	public:
		Win32Window(const WindowAttributes& attr);
		~Win32Window() override;

		virtual void CreateRenderingContext(GDIType GDI) override;
		virtual void MakeRenderingContextCurrent(GDIType GDI) override;

		virtual void OnUpdate() override;

		unsigned int GetWidth() const override { return m_Data.Width; };
		unsigned int GetHeight() const override { return m_Data.Height; };
		std::string GetTitle() const override { return m_Data.Title; };

		virtual void SetEventCallback(const std::function<void(Ref<Events::Event>)>& callback) override { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override { return m_Data.VSync; };

		void* GetNativeWindow() const override { return m_Window; };

	private:
		GLFWwindow* m_Window;
		RenderingContext* m_CurrentRenderingContext;
		std::unordered_map<GDIType, Scope<RenderingContext>> m_RenderingContexts;

		struct WinData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			std::function<void(Ref<Events::Event>)> EventCallback;
		};

		WinData m_Data;

		void Init(const WindowAttributes& attr);
		void SetGLFWEventsCallbacks();
		void ShutDown();

		static void GLFWErrorCallback(int error, const char* msg);
		static uint16_t s_GLFWWindowCount;
		static bool s_GLFWInitialized;
	};
}
