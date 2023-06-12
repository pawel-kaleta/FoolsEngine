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
	fe::Scope<fe::Scene> m_Scene;

	float m_TargetMoveSpeed = 1.1f;
	fe::Set m_Target;
	fe::Set m_ColorQuad;
};