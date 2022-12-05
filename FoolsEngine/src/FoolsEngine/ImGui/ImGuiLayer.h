#pragma once

#include "FE_pch.h"
#include "FoolsEngine\Core\Layer.h"
#include "FoolsEngine\Events\Event.h"
#include "FoolsEngine\Events\MouseEvent.h"
#include "FoolsEngine\Events\KeyEvent.h"
#include "FoolsEngine\Events\ApplicationEvent.h"

#include "ImGuiBackend.h"

namespace fe {

	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate() override;

		void Begin();
		void End();
	private:
		float m_Time = 0.0f;
	};
}