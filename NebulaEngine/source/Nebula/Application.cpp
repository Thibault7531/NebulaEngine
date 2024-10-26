#include "nbpch.h"
#include "Application.h"

#include "Nebula/Events/ApplicationEvent.h"

namespace Nebula {

	Application::Application()
	{
		m_Window = std::unique_ptr<Window>(Window::Create());
	}

	Application::~Application()
	= default;

	void Application::Run()
	{
		while (m_Running) {
			m_Window->OnUpdate();
		}
	}

}