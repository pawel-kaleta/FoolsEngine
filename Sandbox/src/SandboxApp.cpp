#include <FoolsEngine.h>

class LayerExample : public fe::Layer
{
public:
	LayerExample()
		: Layer("LayerExample")
	{ }

	void OnUpdate() override
	{
		FE_PROFILER_FUNC();
		FE_LOG_INFO("LayerExample::OnUpdate()");
	}

	void OnEvent(std::shared_ptr<fe::Event> event)
	{
		FE_LOG_TRACE("{0}", event->ToString());
	}
};

class SandboxApp : public fe::Application
{
public:
	SandboxApp()
	{
		PushInnerLayer(std::make_shared<LayerExample>());
		PushOuterLayer(std::make_shared<fe::ImGuiLayer>());
	}

	~SandboxApp()
	{

	}

};

fe::Application* fe::CreateApplication()
{
	return new SandboxApp();
}