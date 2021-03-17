#pragma once

#ifdef FE_PLATFORM_WINDOWS

extern fe::Application* fe::CreateApplication();

int main(int argc, char** argv)
{
	fe::Log::Init();
	
//#ifdef FE_INTERNAL_BUILD
	fe::tester::Test();
//#endif // FE_INTERNAL_BUILD

	auto app = fe::CreateApplication();

	app->Run();

	delete app;
}

#endif