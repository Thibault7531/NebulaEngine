#pragma once

#include <SDL.h>

#include "../../Nebula/Core/Window.h"
#include "Platform/OpenGL/OpenGLContext.h"

namespace Nebula {
	/**
	 * Windows window.
	 */
	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow() override;

		void OnUpdate() override;

		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }

		inline unsigned int GetTime() const override { return SDL_GetTicks(); }

		inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

		virtual void SetCurrentContext() override;

		inline virtual void* GetNativeWindow() const override { return m_Window; }
		inline virtual uint32_t GetID() const override { return SDL_GetWindowID(m_Window); }

		static bool OnEvent(void* userdata, SDL_Event* e);

	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
	private:
		SDL_Window* m_Window;
		SDL_Event m_Event;

		OpenGLContext* m_Context;

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	};

}
