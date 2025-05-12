#pragma once

#include <FoolsEngine.h>

namespace fe
{
	class PlayViewport
	{
	public:
		PlayViewport();

		void RenderScene();
		void OnImGuiRender();

		void SetScene(const AssetHandle<Scene>& scene) { m_Scene = scene; }

	private:
		AssetHandle<Scene> m_Scene;
		bool m_IsVisible = false;

		glm::vec2 m_ViewportSize = { 0, 0 };
		bool      m_VieportFocus = false;
		bool      m_VieportHover = false;

		Scope<Framebuffer> m_Framebuffer;
	};
}