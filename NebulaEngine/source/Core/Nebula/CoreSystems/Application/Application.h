#pragma once

#include "Nebula/CoreSystems/Core/Core.h"

#include "LayerStack.h"
#include "Nebula/CoreSystems/Windows/Window.h"
#include "Nebula/Events/ApplicationEvent.h"
#include "Nebula/Events/Event.h"
#include "Nebula/ImGui/ImGuiLayer.h"

namespace Nebula {

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

		ImGuiLayer* GetImGuiLayer() const { return m_ImGuiLayer; }

		inline static Application& Get() { return *s_Instance; }

	private:
		bool OnWindowClosed(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
		bool OnWindowMinimize(WindowMinimizeEvent& e);
		bool OnWindowRestored(WindowRestoredEvent& e);

	private:
		Ref<Window> m_MainWindow;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;
		std::chrono::steady_clock::time_point m_LastFrameTime{};

	private:
		static  Application* s_Instance;
	};

	Application* CreateApplication();
}
