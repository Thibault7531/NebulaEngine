#pragma once

#include "Nebula/CoreSystems/Application/Application.h"
#include "Platform/Nebula/CoreSystems/Core/Log.h"

extern Nebula::Application* Nebula::CreateApplication();

// ReSharper disable once CppNonInlineFunctionDefinitionInHeaderFile
int main()
{
	Nebula::Log::Init();
	Nebula::Platform::Log::Init();

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


