#include <FoolsEngine.h>
#include <EntryPoint.h>

#include "LayerExample.h"

class SandboxApp : public fe::Application
{
public:
	SandboxApp()
	{
		PushInnerLayer(std::make_shared<LayerExample>());
	}

	~SandboxApp()
	{

	}

};

fe::Application* fe::CreateApplication()
{
	return new SandboxApp();
}