#pragma once

#include "FoolsEngine\Core\Layer.h"
#include "FoolsEngine\Renderer\1 - Description\Buffer.h"

#include "ImGuiBackend.h"

namespace fe
{
	namespace Events
	{
		class Event;
	}


	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnEvent(Ref<Events::Event> event) override;

		virtual void OnImGuiRender() override {};

		bool BlockEvents = false;
		bool m_Attached = false;

		struct UniformRenderSettings
		{
			float Speed = 0.01f;
			void* MinValue = nullptr;
			void* MaxValue = nullptr;
			const char* Format = nullptr;
			ImGuiSliderFlags Flags = ImGuiSelectableFlags_::ImGuiSelectableFlags_None;
		};

		static bool RenderUniform(const Description::Buffer::Element& uniform, void* uniformDataPtr, const UniformRenderSettings& options = UniformRenderSettings());
	private:
		friend class Application;
		
		void Startup();
		void Shutdown();

		void Begin();
		void End();
	};
}