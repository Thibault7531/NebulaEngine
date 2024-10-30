#pragma once

#include "Core.h"

#include "Events/Event.h"
#include "Window.h"
#include "Events/ApplicationEvent.h"
#include "Nebula/LayerStack.h"

namespace Nebula {

	class NEBULA_API Application
	{
	public:
		Application();
		virtual ~Application();
		
		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
	private:
		bool OnWindowClosed(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
		LayerStack m_LayerStack;
	};

	Application* CreateApplication();
}
