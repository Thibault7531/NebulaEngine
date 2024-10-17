#include "Application.h"

#include "Nebula/Events/ApplicationEvent.h"
#include "Nebula/Log.h"

namespace Nebula {

	Application::Application()
	= default;

	Application::~Application()
	= default;

	void Application::Run()
	{
		WindowResizeEvent e(1280, 720);
		NB_TRACE(e);

		while (true);
	}

}