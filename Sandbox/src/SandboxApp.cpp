#include <FoolsEngine.h>

class SandboxApp : public fe::Application
{
public:
	SandboxApp()
	{

	}

	~SandboxApp()
	{

	}


};

fe::Application* fe::CreateApplication()
{
	return new SandboxApp();
}