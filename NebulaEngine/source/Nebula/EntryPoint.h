#pragma once

#ifdef NB_PLATFORM_WINDOWS

extern Nebula::Application* Nebula::CreateApplication();

int main(int argc, char** argv)
{
	Nebula::Log::Init();
	NB_CORE_WARN("Initialized Log");

	const auto app = Nebula::CreateApplication();
	app->Run();
	delete app;
}

#endif // NB_PLATFORM_WINDOWS

