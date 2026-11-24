#include "Application.h"

#include "Nebula/CoreSystems/Core/Timestep.h"
#include "Nebula/Graphics/Core/RendererAPI.h"
#include "Nebula/Renderer/SceneRenderer.h"
#include "Nebula/Renderer/Renderer2D.h"
#include "Platform/Nebula/CoreSystems/Core/CoreUtils.h"
#include "Platform/Nebula/CoreSystems/Windows/WindowManager.h"
#ifdef NB_PLATFORM_MACOS
#include <Foundation/Foundation.hpp>
#endif

namespace Nebula
{

	Application* Application::s_Instance = nullptr;

	Application::Application(const Platform::WindowProperties& props)
	{
		NB_PROFILE_FUNCTION();

		NB_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		NB_CORE_INFO("Engine starting");

		Platform::CoreUtils::InitThread();

		Platform::GraphicsAPI api = Platform::GraphicsAPI::None;
		if (Platform::Graphics::isGraphicsAPISupported(Platform::GraphicsAPI::Vulkan)) api = Platform::GraphicsAPI::Vulkan;
		else if (Platform::Graphics::isGraphicsAPISupported(Platform::GraphicsAPI::Metal3)) api = Platform::GraphicsAPI::Metal3;
		else NB_CORE_ASSERT(false, "No supported Graphics API found!");

		props.API = api;
		m_MainWindow = Platform::WindowManager::Get().CreateMainWindow(props);
		m_MainWindow->SetEventCallback(NB_BIND_EVENT_FN(OnEvent));

		Graphics::RendererAPI::Init(api);

		m_Device = Graphics::Device::CreateDefaultDevice();
		m_Swapchain = m_Device->CreateSwapchain(m_MainWindow->GetSurface(), Graphics::VSyncMode::Adaptive, {m_MainWindow->GetClientSize().first, m_MainWindow->GetClientSize().second}, 0);

		SceneRenderer::Init();
		Renderer2D::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

		m_MainWindow->Flash();
	}

	Application::~Application()
	{
		NB_PROFILE_FUNCTION();

		NB_CORE_INFO("Engine shutting down");

		PopOverlay(m_ImGuiLayer);
		delete m_ImGuiLayer;
		Renderer2D::Shutdown();
		SceneRenderer::Shutdown();

		Platform::WindowManager::Get().DestroyMainWindow();
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

	void Application::OnEvent(Platform::Event& e)
	{
		NB_PROFILE_FUNCTION();

		Platform::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<Platform::WindowClosedEvent>(NB_BIND_EVENT_FN(OnWindowClosed));
		dispatcher.Dispatch<Platform::WindowResizedEvent>(NB_BIND_EVENT_FN(OnWindowResize));
		dispatcher.Dispatch<Platform::WindowMinimizedEvent>(NB_BIND_EVENT_FN(OnWindowMinimize));
		dispatcher.Dispatch<Platform::WindowRestoredEvent>(NB_BIND_EVENT_FN(OnWindowRestored));
		dispatcher.Dispatch<Platform::WindowRenderEvent>(NB_BIND_EVENT_FN(OnWindowRender));

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
#ifdef NB_PLATFORM_MACOS
			NS::AutoreleasePool* pool = NS::AutoreleasePool::alloc()->init();
#endif

			Render();
#ifdef NB_PLATFORM_MACOS
			pool->release();
#endif

			m_MainWindow->OnUpdate();
		}

		m_Device->WaitIdle();
	}

	void Application::Render()
	{
		m_Swapchain->NewFrame();

		std::chrono::steady_clock::time_point time = std::chrono::steady_clock::now();
		const Timestep timestep = std::chrono::duration_cast<std::chrono::steady_clock::duration>(time - m_LastFrameTime).count() / 1000000000.f;
		m_LastFrameTime = time;

		if (!m_Minimized)
		{
			{
				NB_PROFILE_SCOPE("LayerStack::OnUpdate");
				for (Layer* layer : m_LayerStack)
					layer->OnUpdate(timestep);
			}

			m_ImGuiLayer->Begin(timestep);
			{
				NB_PROFILE_SCOPE("LayerStack::OnImGuiRender");
				for (Layer* layer : m_LayerStack)
					layer->OnImGuiRender();
			}
			m_ImGuiLayer->End();
		}

		m_Swapchain->Present();
	}

	void Application::Shutdown()
	{
		m_Running = false;
	}

	bool Application::OnWindowClosed(Platform::WindowClosedEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(Platform::WindowResizedEvent& e)
	{
		NB_PROFILE_FUNCTION();

		SceneRenderer::OnWindowResize(e.GetWidth(), e.GetHeight());
		m_Swapchain->Resize(e.GetWidth(), e.GetHeight());

		return false;
	}

	bool Application::OnWindowMinimize(Platform::WindowMinimizedEvent& e)
	{
		m_Minimized = true;
		return false;
	}

	bool Application::OnWindowRestored(Platform::WindowRestoredEvent& e)
	{
		m_Minimized = false;
		return false;
	}

	bool Application::OnWindowRender(Platform::WindowRenderEvent& e)
	{
		Render();
		return false;
	}
}
