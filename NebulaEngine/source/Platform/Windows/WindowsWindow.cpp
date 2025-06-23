#include "WindowsWindow.h"

#include "Nebula/Core/Application.h"
#include "Nebula/Events/ApplicationEvent.h"
#include "Nebula/Events/KeyEvent.h"
#include "Nebula/Events/MouseEvent.h"
#include "Platform/OpenGL/OpenGLContext.h"

namespace Nebula {

	static bool s_SDLInitialized = false;

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		NB_PROFILE_FUNCTION();

		WindowsWindow::InitWindow(props);
	}

	void WindowsWindow::InitWindow(const WindowProps& props)
	{
		NB_PROFILE_FUNCTION();

		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;
		m_Data.Maximized = props.Maximized;
		m_Data.Fullscreen = props.Fullscreen;
		m_Data.API = props.API;
		m_Data.EventCallback = std::bind([]{});

		NB_CORE_INFO("Creating window: {0} ({1}, {2})", props.Title, props.Width, props.Height);

		if (!s_SDLInitialized) {
			NB_PROFILE_SCOPE("SDL_Init");

			int success = SDL_Init(SDL_INIT_VIDEO);
			NB_CORE_ASSERT(success, "Could not initialize SDL!");

			SDL_SetEventFilter(OnEvent, nullptr);

			s_SDLInitialized = true;
		}

		{
			NB_PROFILE_SCOPE("SDL_CreateWindow");

			SDL_WindowFlags flags = SDL_WINDOW_RESIZABLE;
			if (!props.Border) flags |= SDL_WINDOW_BORDERLESS;
			if (props.Maximized) flags |= SDL_WINDOW_MAXIMIZED;
			if (props.Fullscreen) flags |= SDL_WINDOW_FULLSCREEN;
			if (props.isResizable) flags |= SDL_WINDOW_RESIZABLE;
			if (props.AlwaysOnTop) flags |= SDL_WINDOW_ALWAYS_ON_TOP;
			if (props.HighDPI) flags |= SDL_WINDOW_HIGH_PIXEL_DENSITY;
			if (props.isUtility) flags |= SDL_WINDOW_UTILITY;
			if (props.API == RendererAPI::API::OpenGL) flags |= SDL_WINDOW_OPENGL;

			m_Window = SDL_CreateWindow(props.Title.c_str(), props.Width, props.Height, flags);
			SDL_PropertiesID windowProps = SDL_GetWindowProperties(m_Window);
			SDL_SetPointerProperty(windowProps, "windowProps", &m_Data);
		}

		if (m_Data.API == RendererAPI::API::OpenGL) {
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

			m_Context = new OpenGLContext(m_Window);
		}
		m_Context->Init();

		SDL_StartTextInput(m_Window);
		
		SetVSync(true);
	}

	void WindowsWindow::Shutdown()
	{
		NB_PROFILE_FUNCTION();

		NB_CORE_INFO("Closing window: {0}", m_Data.Title);

		SDL_DestroyWindow(m_Window);
	}

	void WindowsWindow::OnUpdate()
	{
		NB_PROFILE_FUNCTION();

		m_Context->SwapBuffers();

		if (Application::Get().IsMinimized()) {
			SDL_PumpEvents();
		}

		SDL_Event e;
		while (SDL_PollEvent(&e)) {}
	}

	void WindowsWindow::SetVSync(const bool enabled)
	{
		NB_PROFILE_FUNCTION();

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

	void WindowsWindow::SetFullscreen(bool enabled)
	{
		m_Data.Fullscreen = enabled;
		SDL_SetWindowFullscreen(m_Window, enabled);
	}

	bool WindowsWindow::IsFullscreen() const
	{
		return m_Data.Fullscreen;
	}

	void WindowsWindow::SetMaximized(bool enabled)
	{
		m_Data.Maximized = enabled;
		if (enabled) SDL_MaximizeWindow(m_Window);
		else SDL_RestoreWindow(m_Window);
	}

	void WindowsWindow::Flash() const
	{
		SDL_FlashWindow(m_Window, SDL_FLASH_UNTIL_FOCUSED);
	}

	void WindowsWindow::SetCurrentContext()
	{
		SDL_GL_MakeCurrent(m_Window, m_Context->GetContext());
	}

	bool WindowsWindow::OnEvent(void *userdata, SDL_Event *e)
	{
		Ref<Window> window = GetWindow(e->window.windowID);
		if (window == nullptr) return true;
		if (window != nullptr)
		{
			switch (e->type)
			{
			case SDL_EVENT_WINDOW_RESIZED:
				{
					void *props = SDL_GetPointerProperty(SDL_GetWindowProperties(SDL_GetWindowFromEvent(e)),
														 "windowProps", nullptr);

					if (props != nullptr) {
						WindowData &data = *static_cast<WindowData *>(props);
						data.Width = e->window.data1;
						data.Height = e->window.data2;

						WindowResizeEvent event(data.Width, data.Height);
						event.Window = window.get();
						data.EventCallback(event);

						return false;
					}

					return true;
				}

			case SDL_EVENT_WINDOW_MINIMIZED:
				{
					void *props = SDL_GetPointerProperty(SDL_GetWindowProperties(SDL_GetWindowFromEvent(e)),
														 "windowProps", nullptr);

					if (props != nullptr) {
						WindowData &data = *static_cast<WindowData *>(props);

						WindowMinimizeEvent event{SDL_GetWindowFlags(SDL_GetWindowFromEvent(e)) && SDL_WINDOW_MINIMIZED == 1};
						event.Window = window.get();
						data.EventCallback(event);

						return false;
					}

					return true;
				}

			case SDL_EVENT_WINDOW_MAXIMIZED:
				{
					void *props = SDL_GetPointerProperty(SDL_GetWindowProperties(SDL_GetWindowFromEvent(e)),
														 "windowProps", nullptr);

					if (props != nullptr) {
						WindowData &data = *static_cast<WindowData *>(props);

						WindowMaximizeEvent event{SDL_GetWindowFlags(SDL_GetWindowFromEvent(e)) && SDL_WINDOW_MAXIMIZED == 1};
						event.Window = window.get();
						data.EventCallback(event);

						return false;
					}

					return true;
				}

			case SDL_EVENT_WINDOW_MOVED:
				{
					void *props = SDL_GetPointerProperty(SDL_GetWindowProperties(SDL_GetWindowFromEvent(e)),
														 "windowProps", nullptr);

					if (props != nullptr) {
						WindowData &data = *static_cast<WindowData *>(props);

						WindowMovedEvent event;
						event.Window = window.get();
						data.EventCallback(event);

						return false;
					}

					return true;
				}

			case SDL_EVENT_WINDOW_FOCUS_GAINED:
				{
					void *props = SDL_GetPointerProperty(SDL_GetWindowProperties(SDL_GetWindowFromEvent(e)),
														 "windowProps", nullptr);

					if (props != nullptr) {
						WindowData &data = *static_cast<WindowData *>(props);

						WindowFocusedEvent event;
						event.Window = window.get();
						data.EventCallback(event);

						return false;
					}

					return true;
				}

			case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
				{
					void *props = SDL_GetPointerProperty(SDL_GetWindowProperties(SDL_GetWindowFromEvent(e)),
														 "windowProps", nullptr);

					if (props != nullptr) {
						WindowData &data = *static_cast<WindowData *>(props);

						WindowCloseEvent event;
						event.Window = window.get();
						data.EventCallback(event);

						return false;
					}

					return true;
				}

			case SDL_EVENT_DISPLAY_ORIENTATION:
			case SDL_EVENT_DISPLAY_ADDED:
			case SDL_EVENT_DISPLAY_REMOVED:
			case SDL_EVENT_DISPLAY_MOVED:
			case SDL_EVENT_DISPLAY_CONTENT_SCALE_CHANGED:
				{
					void *props = SDL_GetPointerProperty(SDL_GetWindowProperties(SDL_GetWindowFromEvent(e)),
														 "windowProps", nullptr);

					if (props != nullptr)
					{
						WindowData &data = *static_cast<WindowData *>(props);

						MonitorsChangedEvent event;
						event.Window = window.get();
						data.EventCallback(event);

						return false;
					}

					return true;
				}

			case SDL_EVENT_KEY_DOWN:
				{
					void *props = SDL_GetPointerProperty(SDL_GetWindowProperties(SDL_GetWindowFromEvent(e)),
														 "windowProps", nullptr);

					if (props != nullptr) {
						WindowData &data = *static_cast<WindowData *>(props);

						if (e->key.repeat)
						{
							KeyPressedEvent event(e->key.key, 1);
							event.Window = window.get();
							data.EventCallback(event);
						} else
						{
							KeyPressedEvent event(e->key.key, 0);
							event.Window = window.get();
							data.EventCallback(event);
						}

						return false;
					}

					return true;
				}

			case SDL_EVENT_KEY_UP:
				{
					void *props = SDL_GetPointerProperty(SDL_GetWindowProperties(SDL_GetWindowFromEvent(e)),
														 "windowProps", nullptr);

					if (props != nullptr) {
						WindowData &data = *static_cast<WindowData *>(props);

						KeyReleasedEvent event(e->key.key);
						event.Window = window.get();
						data.EventCallback(event);

						return false;
					}

					return true;
				}

			case SDL_EVENT_TEXT_INPUT:
				{
					void *props = SDL_GetPointerProperty(SDL_GetWindowProperties(SDL_GetWindowFromEvent(e)),
														 "windowProps", nullptr);

					if (props != nullptr) {
						WindowData &data = *static_cast<WindowData *>(props);

						KeyTypedEvent event(*e->text.text);
						event.Window = window.get();
						data.EventCallback(event);

						return false;
					}

					return true;
				}

			case SDL_EVENT_MOUSE_BUTTON_DOWN:
				{
					void *props = SDL_GetPointerProperty(SDL_GetWindowProperties(SDL_GetWindowFromEvent(e)),
														 "windowProps", nullptr);

					if (props != nullptr) {
						WindowData &data = *static_cast<WindowData *>(props);

						MouseButtonPressedEvent event(e->button.button);
						event.Window = window.get();
						data.EventCallback(event);

						return false;
					}

					return true;
				}

			case SDL_EVENT_MOUSE_BUTTON_UP:
				{
					void *props = SDL_GetPointerProperty(SDL_GetWindowProperties(SDL_GetWindowFromEvent(e)),
														 "windowProps", nullptr);

					if (props != nullptr) {
						WindowData &data = *static_cast<WindowData *>(props);

						MouseButtonReleasedEvent event(e->button.button);
						event.Window = window.get();
						data.EventCallback(event);

						return false;
					}

					return true;
				}

			case SDL_EVENT_MOUSE_WHEEL:
				{
					void *props = SDL_GetPointerProperty(SDL_GetWindowProperties(SDL_GetWindowFromEvent(e)),
														 "windowProps", nullptr);

					if (props != nullptr) {
						WindowData &data = *static_cast<WindowData *>(props);

						MouseScrolledEvent event(e->wheel.x, e->wheel.y);
						event.Window = window.get();
						data.EventCallback(event);

						return false;
					}

					return true;
				}

			case SDL_EVENT_MOUSE_MOTION:
				{
					void *props = SDL_GetPointerProperty(SDL_GetWindowProperties(SDL_GetWindowFromEvent(e)),
														 "windowProps", nullptr);

					if (props != nullptr) {
						WindowData &data = *static_cast<WindowData *>(props);

						MouseMovedEvent event(e->motion.x, e->motion.y);
						event.Window = window.get();
						data.EventCallback(event);

						return false;
					}

					return true;
				}

			case SDL_EVENT_WINDOW_MOUSE_ENTER:
				{
					void *props = SDL_GetPointerProperty(SDL_GetWindowProperties(SDL_GetWindowFromEvent(e)),
														 "windowProps", nullptr);

					if (props != nullptr)
					{
						WindowData &data = *static_cast<WindowData *>(props);

						MouseEnteredEvent event;
						event.Window = window.get();
						data.EventCallback(event);

						return false;
					}

					return true;
				}

			case SDL_EVENT_WINDOW_MOUSE_LEAVE:
			{
				void *props = SDL_GetPointerProperty(SDL_GetWindowProperties(SDL_GetWindowFromEvent(e)),
												 "windowProps", nullptr);

				if (props != nullptr)
				{
					WindowData &data = *static_cast<WindowData *>(props);

					MouseLeftEvent event;
					event.Window = window.get();
					data.EventCallback(event);

					return false;
				}

				return true;
			}

				default:
					return true;
			}
		}
		return true;
	}

}
