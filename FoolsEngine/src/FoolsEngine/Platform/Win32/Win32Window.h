#pragma once

#include "FoolsEngine/Platform/Window.h"

#include "FoolsEngine/Renderer/2 - Resource/FrameBuffer.h"

#include <GLFW/glfw3.h>

namespace fe
{
	struct RenderContext;

	class Win32Window : public Window
	{
	public:
		Win32Window(const WindowAttributes& attr, RenderContext* renderContext);
		~Win32Window() override;

		virtual void OnUpdate() override;

		unsigned int GetWidth() const override { return m_Data.Width; };
		unsigned int GetHeight() const override { return m_Data.Height; };
		std::string GetTitle() const override { return m_Data.Title; };

		virtual void SetEventCallback(const std::function<void(Ref<Events::Event>)>& callback) override { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override { return m_Data.VSync; };

		void* GetNativeWindow() const override { return m_Window; };

		GLFWwindow* m_Window;

		struct WinData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			std::function<void(Ref<Events::Event>)> EventCallback;
		};

		WinData m_Data;

		void Init(const WindowAttributes& attr);
		void ShutDown();
	};
}
