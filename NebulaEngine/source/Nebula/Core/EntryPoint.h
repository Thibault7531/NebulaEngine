#pragma once

#ifdef NB_PLATFORM_WINDOWS

/**
 * Creates the application.
 * @return Pointer to the application
 */
extern Nebula::Application* Nebula::CreateApplication();

int main(int argc, char** argv)
{
	Nebula::Log::Init();

	NB_PROFILE_BEGIN_SESSION("Startup", "NebulaProfile-Startup.json");
	const auto app = Nebula::CreateApplication();
	NB_PROFILE_END_SESSION();

	NB_PROFILE_BEGIN_SESSION("Runtime", "NebulaProfile-Runtime.json");
	app->Run();
	NB_PROFILE_END_SESSION();

	NB_PROFILE_BEGIN_SESSION("Shutdown", "NebulaProfile-Shutdown.json");
	delete app;
	NB_PROFILE_END_SESSION();
}

#endif // NB_PLATFORM_WINDOWS

