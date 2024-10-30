#include "nbpch.h"
#include "WindowsWindow.h"

#include "Nebula/Events/ApplicationEvent.h"
#include "Nebula/Events/KeyEvent.h"
#include "Nebula/Events/MouseEvent.h"

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
		m_Data.EventCallback = std::bind([]{});

		NB_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

		if (!s_SDLInitialized) {
			int success = SDL_Init(SDL_INIT_VIDEO);
			NB_CORE_ASSERT(success, "Could not initialize SDL!");

			s_SDLInitialized = true;
		}

		m_Window = SDL_CreateWindow(props.Title.c_str(), props.Width, props.Height, SDL_WINDOW_RESIZABLE);
		SDL_PropertiesID windowProps = SDL_GetWindowProperties(m_Window);
		SDL_SetPointerProperty(windowProps, "windowProps", &m_Data);

		m_Context = SDL_GL_CreateContext(m_Window);
		SDL_GL_MakeCurrent(m_Window, m_Context);
		
		SetVSync(true);

		SDL_SetEventFilter([](void* userdata, SDL_Event* e)
		{
			switch (e->type)
			{
			case SDL_EVENT_WINDOW_RESIZED:
				{
					WindowData& data = *(WindowData*)SDL_GetPointerProperty(SDL_GetWindowProperties(SDL_GetWindowFromEvent(e)), "windowProps", "");
					data.Width = e->window.data1;
					data.Height = e->window.data2;

					WindowResizeEvent event(data.Width, data.Height);
					data.EventCallback(event);

					return false;
				}

			case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
				{
					WindowData& data = *(WindowData*)SDL_GetPointerProperty(SDL_GetWindowProperties(SDL_GetWindowFromEvent(e)), "windowProps", "");

					WindowCloseEvent event;
					data.EventCallback(event);

					return false;
				}

			case SDL_EVENT_KEY_DOWN:
				{
					WindowData& data = *(WindowData*)SDL_GetPointerProperty(SDL_GetWindowProperties(SDL_GetWindowFromEvent(e)), "windowProps", "");

					switch (e->key.repeat)
					{
					case false:
						{
							KeyPressedEvent event(e->key.key, 0);
							data.EventCallback(event);
							break;
						}
					case true:
						{
							KeyPressedEvent event(e->key.key, 1);
							data.EventCallback(event);
							break;
						}
					}

					return false;
				}

			case SDL_EVENT_KEY_UP:
				{
					WindowData& data = *(WindowData*)SDL_GetPointerProperty(SDL_GetWindowProperties(SDL_GetWindowFromEvent(e)), "windowProps", "");

					KeyReleasedEvent event(e->key.key);
					data.EventCallback(event);

					return false;
				}

			case SDL_EVENT_MOUSE_BUTTON_DOWN:
				{
					WindowData& data = *(WindowData*)SDL_GetPointerProperty(SDL_GetWindowProperties(SDL_GetWindowFromEvent(e)), "windowProps", "");

					MouseButtonPressedEvent event(e->button.button);
					data.EventCallback(event);

					return false;
				}

			case SDL_EVENT_MOUSE_BUTTON_UP:
				{
					WindowData& data = *(WindowData*)SDL_GetPointerProperty(SDL_GetWindowProperties(SDL_GetWindowFromEvent(e)), "windowProps", "");

					MouseButtonReleasedEvent event(e->button.button);
					data.EventCallback(event);

					return false;
				}

			case SDL_EVENT_MOUSE_WHEEL:
				{
					WindowData& data = *(WindowData*)SDL_GetPointerProperty(SDL_GetWindowProperties(SDL_GetWindowFromEvent(e)), "windowProps", "");

					MouseScrolledEvent event(e->wheel.x, e->wheel.y);
					data.EventCallback(event);

					return false;
				}

			case SDL_EVENT_MOUSE_MOTION:
				{
					WindowData& data = *(WindowData*)SDL_GetPointerProperty(SDL_GetWindowProperties(SDL_GetWindowFromEvent(e)), "windowProps", "");

					MouseMovedEvent event(e->motion.x, e->motion.y);
					data.EventCallback(event);

					return false;
				}

			default:
				return true;
			}
		}
			, nullptr);
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
