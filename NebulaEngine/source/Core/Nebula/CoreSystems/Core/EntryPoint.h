#pragma once

#include "Nebula/CoreSystems/Application/Application.h"
#include "Platform/Nebula/CoreSystems/Core/Log.h"

extern Nebula::Application* Nebula::CreateApplication();

// ReSharper disable once CppNonInlineFunctionDefinitionInHeaderFile
int main()
{
	Nebula::Log::Init();
	Nebula::Platform::Log::Init();

#ifdef NB_PLATFORM_APP_NAME
	Nebula::Platform::Files::setAppName(NB_PLATFORM_APP_NAME);
	NB_CORE_DEBUG("Application data directory: {}", Nebula::Platform::Files::GetUserDataPath().string());
#endif

	NB_PROFILE_BEGIN_SESSION("Startup", (Nebula::Platform::Files::GetUserDataPath() / "NebulaProfile-Startup.json").string());
	const auto app = Nebula::CreateApplication();
	NB_PROFILE_END_SESSION();

	NB_PROFILE_BEGIN_SESSION("Runtime", (Nebula::Platform::Files::GetUserDataPath() / "NebulaProfile-Runtime.json").string());
	app->Run();
	NB_PROFILE_END_SESSION();

	NB_PROFILE_BEGIN_SESSION("Shutdown", (Nebula::Platform::Files::GetUserDataPath() / "NebulaProfile-Shutdown.json").string());
	delete app;
	NB_PROFILE_END_SESSION();
}


