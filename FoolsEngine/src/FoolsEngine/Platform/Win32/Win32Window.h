#pragma once

#include "FoolsEngine/Platform/Window.h"
#include "FoolsEngine/Platform/Events/Event.h"

#include "FoolsEngine/Foundation/Utils/Core.h"

#include <functional>

struct GLFWwindow;

namespace fe
{
	class Win32Window final : public Window
	{
	public:
		void Init(const Attributes& attr) final override;
		void Create() final override;
		void Destroy() final override;

		void PollEvents() final override;
		void SwapBuffers() final override;

		virtual void SetVSync(bool enabled) final override;

		virtual void* GetNativeWindow() const final override { return m_Window; };
		
		GLFWwindow* m_Window;
	};
}
