#include <FoolsEngine.h>
#include <EntryPoint.h>

#include "LayerExample.h"

class SandboxApp : public fe::Application
{
public:
	SandboxApp(const fe::ApplicationSpecification& spec)
		: fe::Application(spec)
	{
		PushInnerLayer(fe::CreateScope<LayerExample>());
	}

	~SandboxApp()
	{

	}

};

fe::Application* fe::CreateApplication(const ApplicationCommandLineArgs& args)
{
	fe::ApplicationSpecification app_spec;

	app_spec.CommandLineArgs = args;
	app_spec.Name = "Sandbox - FoolsEngine Application";
	app_spec.WindowAttributes = { "Sandbox - FoolsEngine Application", 1920, 1080, GDIType::OpenGL };

	return new SandboxApp(app_spec);
}