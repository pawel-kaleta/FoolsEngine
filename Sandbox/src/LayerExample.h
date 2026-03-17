#pragma once

#include <FoolsEngine.h>

class LayerExample : public fe::Layer
{
public:
	LayerExample();

	virtual void OnAttach() override;

	void OnUpdate() override;
	void OnEvent(fe::Ref<fe::Events::Event> event) override;
	void OnKeyPressedEvent(fe::Ref<fe::Events::KeyPressedEvent> event);
	void OnImGuiRender() override;

private:
	fe::AssetHandle<fe::Scene> m_Scene;
	fe::Scope<fe::Resource::Framebuffer_OpenGL> m_Framebuffer;
	struct { uint32_t x; uint32_t y; } m_FramebufferSize = { 0, 0 };
};