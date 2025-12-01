#include <FoolsEngine.h>
#include <EntryPoint.h>

#include "LayerExample.h"
#include "GameCode.h"

class SandboxApp : public fe::Application
{
public:
	SandboxApp(const fe::ApplicationSpecification& spec)
		: fe::Application(spec)
	{ }

	virtual ~SandboxApp() = default;

	virtual bool ClientAppProjectInit() override;
	virtual void ClientAppStartup() override;
	virtual void ClientAppShutdown() override;

	fe::Ref<LayerExample> m_MainLayer;
};

fe::Application* fe::CreateApplication(const ApplicationCommandLineArgs& args)
{
	fe::ApplicationSpecification app_spec;

	app_spec.CommandLineArgs = args;
	app_spec.Name = "Sandbox - FoolsEngine Application";
	app_spec.WindowAttributes = { "Sandbox - FoolsEngine Application", 1920, 1080, GAPIType::OpenGL };

	return new SandboxApp(app_spec);
}

bool SandboxApp::ClientAppProjectInit()
{
	Application::ProjectLoad("SandboxProject.feproj");
	PushInnerLayer(m_MainLayer);
	return false;
}

void SandboxApp::ClientAppStartup()
{
	fe::RegisterGameplayTypes();
	m_MainLayer = fe::CreateRef<LayerExample>();
}

void SandboxApp::ClientAppShutdown()
{

}
