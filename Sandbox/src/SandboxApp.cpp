#include <FoolsEngine.h>
#include <EntryPoint.h>

#include "LayerExample.h"

class SandboxApp : public fe::Application
{
public:
	SandboxApp()
	{
		PushInnerLayer(fe::CreateScope<LayerExample>());
	}

	~SandboxApp()
	{

	}

};

fe::Application* fe::CreateApplication()
{
	return new SandboxApp();
}