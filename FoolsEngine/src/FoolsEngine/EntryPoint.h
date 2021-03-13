#pragma once

#ifdef FE_PLATFORM_WINDOWS

extern fe::Application* fe::CreateApplication();

int main(int argc, char** argv)
{
	auto app = fe::CreateApplication();

	app->Run();

	delete app;
}

#endif