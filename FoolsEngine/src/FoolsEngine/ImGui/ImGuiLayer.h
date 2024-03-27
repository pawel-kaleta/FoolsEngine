#pragma once

#include "FE_pch.h"
#include "FoolsEngine\Core\Layer.h"
#include "FoolsEngine\Events\Event.h"
#include "FoolsEngine\Events\MouseEvent.h"
#include "FoolsEngine\Events\KeyEvent.h"
#include "FoolsEngine\Events\ApplicationEvent.h"

#include "ImGuiBackend.h"

#include "FoolsEngine\Renderer\1 - Primitives\Uniform.h"

namespace fe {

	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnEvent(Ref<Events::Event> event) override;

		virtual void OnImGuiRender() override {};

		void Begin();
		void End();

		void BlockEvents(bool block) { m_BlockEvents = block; }
		bool IsBlocking() const { return m_BlockEvents; }

		struct UniformRenderSettings
		{
			float Speed = 0.01f;
			void* MinValue = nullptr;
			void* MaxValue = nullptr;
			const char* Format = nullptr;
			ImGuiSliderFlags Flags = ImGuiSelectableFlags_::ImGuiSelectableFlags_None;
		};

		static void RenderUniform(const Uniform& uniform, void* uniformDataPtr, const UniformRenderSettings& options = UniformRenderSettings());
	private:
		float m_Time = 0.0f;
		bool m_BlockEvents = false;
	};
}