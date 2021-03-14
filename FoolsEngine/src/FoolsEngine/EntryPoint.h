#pragma once

#ifdef FE_PLATFORM_WINDOWS

extern fe::Application* fe::CreateApplication();



int main(int argc, char** argv)
{
	fe::Log::Init();
	fe::Log::UnitTest();

	auto app = fe::CreateApplication();

	app->Run();

	delete app;
}

#endif