#include "Application.h"

#include "Nebula/CoreSystems/Core/Timestep.h"
#include "Nebula/CoreSystems/Windows/WindowManager.h"
#include "Nebula/Renderer/Renderer.h"

namespace Nebula
{
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application(const WindowProps& props)
	{
		NB_PROFILE_FUNCTION();

		NB_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		NB_CORE_INFO("Engine starting");

		m_MainWindow = WindowManager::Get().CreateNewMainWindow(props);
		m_MainWindow->SetEventCallback(BIND_EVENT_FN(OnEvent));

		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

		m_MainWindow->Flash();
	}

	Application::~Application()
	{
		NB_PROFILE_FUNCTION();

		NB_CORE_INFO("Engine shutting down");

		Renderer::Shutdown();
		PopOverlay(m_ImGuiLayer);

		WindowManager::Get().DestroyMainWindow();
		m_MainWindow.reset();
	}

	void Application::PushLayer(Layer* layer)
	{
		NB_PROFILE_FUNCTION();

		NB_CORE_DEBUG("Layer added to stack: {0}", layer->GetName());

		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PopLayer(Layer* layer)
	{
		NB_PROFILE_FUNCTION();

		NB_CORE_DEBUG("Layer removed from stack: {0}", layer->GetName());

		m_LayerStack.PopLayer(layer);
		layer->OnDetach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		NB_PROFILE_FUNCTION();

		NB_CORE_DEBUG("Overlay added to stack: {0}", layer->GetName());

		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	void Application::PopOverlay(Layer* layer)
	{
		NB_PROFILE_FUNCTION();

		NB_CORE_DEBUG("Overlay removed from stack: {0}", layer->GetName());

		m_LayerStack.PopOverlay(layer);
		layer->OnDetach();
	}

	void Application::OnEvent(Event& e)
	{
		NB_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClosed));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));
		dispatcher.Dispatch<WindowMinimizeEvent>(BIND_EVENT_FN(OnWindowMinimize));
		dispatcher.Dispatch<WindowRestoredEvent>(BIND_EVENT_FN(OnWindowRestored));

		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
		{
			if (e.Handled)
				break;
			(*it)->OnEvent(e);
		}
	}

	void Application::Run()
	{
		NB_PROFILE_FUNCTION();

		while (m_Running) {
			NB_PROFILE_SCOPE("RunLoop");

			m_MainWindow->SetCurrentContext();

			std::chrono::steady_clock::time_point time = std::chrono::steady_clock::now();
			const Timestep timestep = std::chrono::duration_cast<std::chrono::steady_clock::duration>(time - m_LastFrameTime).count() / 1000000000;
			m_LastFrameTime = time;

			if (!m_Minimized)
			{
				{
					NB_PROFILE_SCOPE("LayerStack::OnUpdate");
					for (Layer* layer : m_LayerStack)
						layer->OnUpdate(timestep);
				}

				m_ImGuiLayer->Begin();
				{
					NB_PROFILE_SCOPE("LayerStack::OnImGuiRender");
					for (Layer* layer : m_LayerStack)
						layer->OnImGuiRender();
				}
				m_ImGuiLayer->End();
			}

			m_MainWindow->OnUpdate();
		}
	}

	void Application::Shutdown()
	{
		m_Running = false;
	}

	bool Application::OnWindowClosed(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		NB_PROFILE_FUNCTION();

		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

		return false;
	}

	bool Application::OnWindowMinimize(WindowMinimizeEvent& e)
	{
		m_Minimized = true;
		return false;
	}

	bool Application::OnWindowRestored(WindowRestoredEvent& e)
	{
		m_Minimized = false;
		return false;
	}

}
