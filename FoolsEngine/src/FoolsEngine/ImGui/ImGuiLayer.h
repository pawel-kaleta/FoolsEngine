#pragma once

#include "FE_pch.h"
#include "FoolsEngine\Core\Layer.h"

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
		float m_time = 0.0f;
	};
}