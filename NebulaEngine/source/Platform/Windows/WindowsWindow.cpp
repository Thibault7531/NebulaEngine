#include "nbpch.h"
#include "WindowsWindow.h"

#include "glad/glad.h"
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

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

		m_Window = SDL_CreateWindow(props.Title.c_str(), props.Width, props.Height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
		SDL_PropertiesID windowProps = SDL_GetWindowProperties(m_Window);
		SDL_SetPointerProperty(windowProps, "windowProps", &m_Data);

		m_Context = SDL_GL_CreateContext(m_Window);
		SDL_GL_MakeCurrent(m_Window, m_Context);

		int status = gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);
		NB_CORE_ASSERT(status, "Failed to initialize Glad!");

		SDL_StartTextInput(m_Window);
		
		SetVSync(true);

		SDL_SetEventFilter([](void* userdata, SDL_Event* e)
		{
			if ((SDL_Window*)userdata == SDL_GetWindowFromEvent(e))
			{
				switch (e->type)
				{
				case SDL_EVENT_WINDOW_RESIZED:
					{
						void* props = SDL_GetPointerProperty(SDL_GetWindowProperties(SDL_GetWindowFromEvent(e)), "windowProps", nullptr);

						if (props != nullptr)
						{
							WindowData& data = *(WindowData*)props;
							data.Width = e->window.data1;
							data.Height = e->window.data2;

							WindowResizeEvent event(data.Width, data.Height);
							data.EventCallback(event);

							return false;
						}

						return true;
					}

				case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
					{
						void* props = SDL_GetPointerProperty(SDL_GetWindowProperties(SDL_GetWindowFromEvent(e)), "windowProps", nullptr);

						if (props != nullptr)
						{
							WindowData& data = *(WindowData*)props;

							WindowCloseEvent event;
							data.EventCallback(event);

							return false;
						}

						return true;
					}

				case SDL_EVENT_KEY_DOWN:
					{
						void* props = SDL_GetPointerProperty(SDL_GetWindowProperties(SDL_GetWindowFromEvent(e)), "windowProps", nullptr);

						if (props != nullptr)
						{
							WindowData& data = *(WindowData*)props;

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

						return true;
					}

				case SDL_EVENT_KEY_UP:
					{
						void* props = SDL_GetPointerProperty(SDL_GetWindowProperties(SDL_GetWindowFromEvent(e)), "windowProps", nullptr);

						if (props != nullptr)
						{
							WindowData& data = *(WindowData*)props;

							KeyReleasedEvent event(e->key.key);
							data.EventCallback(event);

							return false;
						}

						return true;
					}

				case SDL_EVENT_TEXT_INPUT:
					{
						void* props = SDL_GetPointerProperty(SDL_GetWindowProperties(SDL_GetWindowFromEvent(e)), "windowProps", nullptr);

						if (props != nullptr)
						{
							WindowData& data = *(WindowData*)props;

							KeyTypedEvent event(*e->text.text);
							data.EventCallback(event);

							return false;
						}

						return true;
					}

				case SDL_EVENT_MOUSE_BUTTON_DOWN:
					{
						void* props = SDL_GetPointerProperty(SDL_GetWindowProperties(SDL_GetWindowFromEvent(e)), "windowProps", nullptr);

						if (props != nullptr)
						{
							WindowData& data = *(WindowData*)props;

							MouseButtonPressedEvent event(e->button.button);
							data.EventCallback(event);

							return false;
						}

						return true;
					}

				case SDL_EVENT_MOUSE_BUTTON_UP:
					{
						void* props = SDL_GetPointerProperty(SDL_GetWindowProperties(SDL_GetWindowFromEvent(e)), "windowProps", nullptr);

						if (props != nullptr)
						{
							WindowData& data = *(WindowData*)props;

							MouseButtonReleasedEvent event(e->button.button);
							data.EventCallback(event);

							return false;
						}

						return true;
					}

				case SDL_EVENT_MOUSE_WHEEL:
					{
						void* props = SDL_GetPointerProperty(SDL_GetWindowProperties(SDL_GetWindowFromEvent(e)), "windowProps", nullptr);

						if (props != nullptr)
						{
							WindowData& data = *(WindowData*)props;

							MouseScrolledEvent event(e->wheel.x, e->wheel.y);
							data.EventCallback(event);

							return false;
						}

						return true;
					}

				case SDL_EVENT_MOUSE_MOTION:
					{
						void* props = SDL_GetPointerProperty(SDL_GetWindowProperties(SDL_GetWindowFromEvent(e)), "windowProps", nullptr);

						if (props != nullptr)
						{
							WindowData& data = *(WindowData*)props;

							MouseMovedEvent event(e->motion.x, e->motion.y);
							data.EventCallback(event);

							return false;
						}

						return true;
					}

				default:
					return true;
				}
			} else return true;
		}
			, m_Window);
	}

	void WindowsWindow::Shutdown()
	{
		SDL_DestroyWindow(m_Window);
	}

	void WindowsWindow::OnUpdate()
	{
		SDL_GL_SwapWindow(m_Window);
		glClear(GL_COLOR_BUFFER_BIT);
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
