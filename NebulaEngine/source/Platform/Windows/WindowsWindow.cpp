#include "nbpch.h"
#include "WindowsWindow.h"

namespace Nebula {

	static bool s_SDLInitialized = false;

	Window* Window::Create(const WindowProps& props)
	{
		return new WindowsWindow(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}

	void WindowsWindow::Init(const WindowProps& props)
	{
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		NB_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

		if (!s_SDLInitialized) {
			int success = SDL_Init(SDL_INIT_VIDEO);
			NB_CORE_ASSERT(success, "Could not initialize SDL!");

			s_SDLInitialized = true;
		}


		SDL_PropertiesID SDLProps = SDL_CreateProperties();
		SDL_SetPointerProperty(SDLProps, "props", &m_Data);
		m_Window = SDL_CreateWindowWithProperties(SDLProps);

		SDL_SetWindowTitle(m_Window, props.Title.c_str());
		SDL_SetWindowSize(m_Window, props.Width, props.Height);

		m_Context = SDL_GL_CreateContext(m_Window);
		SDL_GL_MakeCurrent(m_Window, m_Context);
		
		SetVSync(true);
	}

	void WindowsWindow::Shutdown()
	{
		SDL_DestroyWindow(m_Window);
	}

	void WindowsWindow::OnUpdate()
	{
		while (SDL_PollEvent(&m_Event));
		SDL_GL_SwapWindow(m_Window);
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		if (enabled)
			SDL_GL_SetSwapInterval(1);
		else
			SDL_GL_SetSwapInterval(0);

		m_Data.VSync = enabled;
	}

	bool WindowsWindow::IsVSync() const
	{
		return m_Data.VSync;
	}

}