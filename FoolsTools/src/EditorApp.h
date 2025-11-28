#pragma once

#include <FoolsEngine.h>

namespace fe
{
	class EditorLayer;

	class EditorApp : public Application
	{
	public:
		EditorApp(const ApplicationSpecification& spec);
		virtual ~EditorApp() = default;

		virtual void ClientAppStartup() override;
		virtual void ClientAppShutdown() override;
		virtual bool ClientAppProjectInit() override;

		static EditorApp& Get() { return (EditorApp&)Application::Get(); }

	private:
		Ref<EditorLayer> m_EditorLayer;
		bool m_LayerAttached = false;
	};
}