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

		void SetScene(const Ref<Scene>& scene) { m_Scene = scene; }

	private:
		Ref<Scene> m_Scene;
		bool m_IsVisible;

		glm::vec2 m_ViewportSize = { 0, 0 };
		bool      m_VieportFocus = false;
		bool      m_VieportHover = false;

		Scope<Framebuffer> m_Framebuffer;
	};
}