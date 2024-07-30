#pragma once

#include <FoolsEngine.h>

namespace fe
{
	class EditorLayer;

	class EditorApp : public Application
	{
	public:
		EditorApp(const std::string& name);
		virtual ~EditorApp() = default;

		virtual void ClientAppStartup() override;
		virtual void ClientAppShutdown() override;

		static EditorApp& Get() { return (EditorApp&)Application::Get(); }

	private:
		friend class EditorAssetManager;
		Scope<EditorAssetManager> m_EditorAssetManager;

		Ref<EditorLayer> m_EditorLayer;
	};

	
}