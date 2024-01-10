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
	fe::Ref<fe::Scene> m_Scene;

	fe::Entity m_ColorSprite;
};