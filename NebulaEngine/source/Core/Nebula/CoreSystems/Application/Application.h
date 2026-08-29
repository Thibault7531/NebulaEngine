#pragma once


#include "LayerStack.h"
#include "Nebula/CoreSystems/Core/Core.h"
#include "Nebula/Graphics/Device.h"
#include "Nebula/ImGui/ImGuiLayer.h"
#include "Nebula/Graphics/Buffers/Framebuffer.h"
#include "Platform/Nebula/CoreSystems/Events/WindowEvents.h"
#include "Platform/Nebula/CoreSystems/Windows/Window.h"

namespace Nebula {

	class Application
	{
	public:
		Application(const Platform::WindowProperties& props = Platform::WindowProperties());
		virtual ~Application();

		void Run();
		void Render();
		void Shutdown();

		void OnEvent(Platform::Event& e);

		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);
		void PushOverlay(Layer* layer);
		void PopOverlay(Layer* layer);

		Ref<Graphics::Device> GetDevice() const { return m_Device; }
		Ref<Graphics::Framebuffer> GetMainFramebuffer() const { return m_MainFramebuffer; }
		ImGuiLayer* GetImGuiLayer() const { return m_ImGuiLayer; }

		inline static Application& Get() { return *s_Instance; }

	private:
		bool OnWindowClosed(Platform::WindowClosedEvent& e);
		bool OnWindowResize(Platform::WindowResizedEvent& e);
		bool OnWindowMinimize(Platform::WindowMinimizedEvent& e);
		bool OnWindowRestored(Platform::WindowRestoredEvent& e);
		bool OnWindowRender(Platform::WindowRenderEvent& e);

	private:
		Ref<Platform::Window> m_MainWindow;
		Ref<Graphics::Device> m_Device;
		Ref<Graphics::Framebuffer> m_MainFramebuffer;
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
