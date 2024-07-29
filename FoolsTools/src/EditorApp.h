#pragma once

#include <FoolsEngine.h>

namespace fe
{
	class EditorApp : public Application
	{
	public:
		EditorApp(const std::string& name);

		virtual ~EditorApp() = default;

		static EditorApp& Get() { return (EditorApp&)Application::Get(); }

	private:
		friend class EditorAssetManager;
		Scope<EditorAssetManager> m_EditorAssetManager;
	};

	
}