#pragma once

#include "Core.h"

#include "Window.h"
#include "Timestep.h"
#include "../Events/ApplicationEvent.h"
#include "../Events/Event.h"
#include "../ImGui/ImGuiLayer.h"
#include "LayerStack.h"

namespace Nebula {

	//TODO: Clean application class for more modularity and easier maintenance
	class Application
	{
	public:
		Application(const WindowProps& props = WindowProps());
		virtual ~Application();

		void Run();
		void Shutdown();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);
		void PushOverlay(Layer* layer);
		void PopOverlay(Layer* layer);

		inline Window& GetWindow() const { return *m_Window; }

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

		bool IsMinimized() const { return m_Minimized; }

		inline static Application& Get() { return *s_Instance; }

	private:
		bool OnWindowClosed(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
		bool OnWindowMinimize(WindowMinimizeEvent& e);
		bool OnWindowRestored(WindowRestoredEvent& e);

	private:
		Ref<Window> m_Window;
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
