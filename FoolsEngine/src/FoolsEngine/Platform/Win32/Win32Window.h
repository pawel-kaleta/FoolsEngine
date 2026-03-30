#pragma once

#include "FoolsEngine/Platform/Window.h"

#include "FoolsEngine/Renderer/2 - Resource/FrameBuffer.h"
#include "FoolsEngine/Renderer/5 - Render Context/RenderingContext.h"

#include <GLFW/glfw3.h>

namespace fe
{
	class Win32Window : public Window
	{
	public:
		Win32Window(const WindowAttributes& attr);
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
		Resource::FramebufferBase* m_OutputFramebuffer;

		//RenderingContext* m_CurrentRenderingContext;
		//std::unordered_map<GAPIType::ValueType, Scope<RenderingContext>> m_RenderingContexts;

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

		virtual void CommitOutputBuffer() const;
	};
}
