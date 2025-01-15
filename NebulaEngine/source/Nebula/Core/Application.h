#pragma once

#include "Core.h"

#include "Window.h"
#include "Timestep.h"
#include "../Events/ApplicationEvent.h"
#include "../Events/Event.h"
#include "../ImGui/ImGuiLayer.h"
#include "LayerStack.h"

namespace Nebula {
	/**
	 * Class representing the application.
	 */
	class Application
	{
	public:
		Application();
		virtual ~Application();

		/**
		 * Runs the application.
		 */
		void Run();

		/**
		 * Dispatches an event through the application.
		 * @param e Event to be handled
		 */
		void OnEvent(Event& e);

		/**
		 * Adds a layer to the stack.
		 * @param layer Layer to be added
		 */
		void PushLayer(Layer* layer);
		/**
		 * Adds a layer on top of the stack.
		 * @param layer Layer to be added on top
		 */
		void PushOverlay(Layer* layer);

		/**
		 * Gets a pointer to the main window.
		 * @return Pointer to the main application window
		 */
		inline Window& GetWindow() const { return *m_Window; }

		/**
		 * Gets a pointer to the application.
		 * @return Pointer to the application
		 */
		inline static Application& Get() { return *s_Instance; }

	private:
		bool OnWindowClosed(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
		bool OnWindowMinimize(WindowMinimizeEvent& e);
		bool OnWindowMaximize(WindowMaximizeEvent& e);

	private:
		Scope<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;

	private:
		static  Application* s_Instance;
	};

	/**
	 * Creates the application.
	 * @return Pointer to the application
	 */
	Application* CreateApplication();
}
