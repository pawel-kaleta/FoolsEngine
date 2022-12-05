#pragma once

#include "FE_pch.h"
#include "FoolsEngine\Core\Layer.h"
#include "FoolsEngine\Events\Event.h"
#include "FoolsEngine\Events\MouseEvent.h"
#include "FoolsEngine\Events\KeyEvent.h"
#include "FoolsEngine\Events\ApplicationEvent.h"

namespace fe {

	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate() override;
		void OnEvent(std::shared_ptr<Event> event) override;

	private:
		bool OnMouseButtonPressedEvent(std::shared_ptr < MouseButtonPressedEvent > e);
		bool OnMouseButtonReleasedEvent(std::shared_ptr < MouseButtonReleasedEvent > e);
		bool OnMouseMovedEvent(std::shared_ptr < MouseMovedEvent > e);
		bool OnMouseScrolledEvent(std::shared_ptr < MouseScrolledEvent > e);
		
		bool OnKeyPressedEvent(std::shared_ptr < KeyPressedEvent > e);
		bool OnKeyReleasedEvent(std::shared_ptr < KeyReleasedEvent > e);
		bool OnKeyTypedEvent(std::shared_ptr < KeyTypedEvent > e);
		
		bool OnWindowResizeEvent(std::shared_ptr < WindowResizeEvent > e);

		float m_Time = 0.0f;
	};
}