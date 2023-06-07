#include <FoolsEngine.h>
#include <EntryPoint.h>

#include "LayerExample.h"

class SandboxApp : public fe::Application
{
public:
	SandboxApp(const std::string& name)
		:Application(name)
	{
		PushInnerLayer(fe::CreateScope<LayerExample>());
	}

	~SandboxApp()
	{

	}

};

fe::Application* fe::CreateApplication()
{
	return new SandboxApp("Sandbox - FoolsEngine Application");
}