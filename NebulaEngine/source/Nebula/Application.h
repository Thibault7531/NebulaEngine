#pragma once

#include "Core.h"

#include "Events/Event.h"
#include "Window.h"
#include "Events/ApplicationEvent.h"
#include "ImGui/ImGuiLayer.h"
#include "Nebula/LayerStack.h"
#include "Renderer/Buffer.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexArray.h"

namespace Nebula {

	class Application
	{
	public:
		Application();
		virtual ~Application();
		
		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline Window& GetWindow() { return *m_Window; }

		inline static Application& Get() { return *s_Instance; }
	private:
		bool OnWindowClosed(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		LayerStack m_LayerStack;

		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<VertexBuffer> m_VertexBuffer;
		std::unique_ptr<IndexBuffer> m_IndexBuffer;
		std::unique_ptr<VertexArray> m_VertexArray;
	private:
		static  Application* s_Instance;
	};

	Application* CreateApplication();
}
