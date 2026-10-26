#pragma once

#include "FoolsEngine/Renderer/1 - GAPI/Context.h"

namespace fe
{
	namespace Events
	{
		class Event;
	}


	class Window
	{
	public:
		struct Attributes
		{
			String Title;
			UInt Width;
			UInt Height;
			Render::GAPI::Platform GAPI;

			void Init()
			{
				Title.Init();
				Width = -1;
				Height = -1;
				GAPI = Render::GAPI::Platform::None;
			}
		};

		virtual void Init(const Attributes& attr) = 0;
		virtual void Create() = 0;
		virtual void Destroy() = 0;

		virtual void PollEvents() = 0;
		virtual void SwapBuffers() = 0;

		virtual void SetVSync(bool enabled) = 0;
		virtual void* GetNativeWindow() const = 0;

		Attributes mAttributes;
		bool mVSync;
		std::function<void(Ref<Events::Event>)> mEventCallback;
	};
}