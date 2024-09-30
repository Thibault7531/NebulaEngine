#pragma once

#ifdef NB_PLATFORM_WINDOWS

extern Nebula::Application* Nebula::CreateApplication();

int main(int argc, char** argv)
{
	Nebula::Log::Init();
	NB_CORE_WARN("Initialized Log");
	int a = 69;
	NB_INFO("Hello World! Var={0}", a);

	auto app = Nebula::CreateApplication();
	app->Run();
	delete app;
}

#endif // NB_PLATFORM_WINDOWS

