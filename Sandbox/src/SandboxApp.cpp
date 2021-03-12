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

int main()
{
	SandboxApp* sandbox = new SandboxApp();

	sandbox->Run();

	delete sandbox;
}