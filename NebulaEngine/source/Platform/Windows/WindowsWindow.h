#pragma once

#include <SDL.h>

#include "../../Nebula/Core/Window.h"
#include "Platform/OpenGL/OpenGLContext.h"

namespace Nebula {

	// TODO: Migrate to Win32 API
	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow() override = default;

		virtual void Shutdown() override;

		virtual void OnUpdate() override;

		[[nodiscard]] virtual inline unsigned int GetWidth() const override { return m_Data.Width; }
		[[nodiscard]] virtual inline unsigned int GetHeight() const override { return m_Data.Height; }
		[[nodiscard]] virtual unsigned int GetPosX() const override;
		[[nodiscard]] virtual unsigned int GetPosY() const override;

		[[nodiscard]] virtual inline unsigned int GetTime() const override { return SDL_GetTicks(); }

		virtual inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		virtual inline void SetWindowHitTest(std::function<HitTest(int, int)> callback) override {m_Data.HitTestCallback = callback;};
		virtual void SetVSync(bool enabled) override;
		[[nodiscard]] virtual bool IsVSync() const override;
		virtual void SetFullscreen(bool enabled) override;
		[[nodiscard]] virtual bool IsFullscreen() const override;
		virtual void SetMaximized(bool enabled) override;
		[[nodiscard]] virtual bool IsMaximized() const override { return m_Data.Maximized; }
		virtual void Minimize() override;

		virtual void Flash() const override;

		virtual void SetCurrentContext() override;

		[[nodiscard]] inline virtual void* GetNativeWindow() const override { return m_Window; }
		[[nodiscard]] inline virtual uint32_t GetID() const override { return SDL_GetWindowID(m_Window); }

		static bool OnEvent(void* userdata, SDL_Event* e);
		static SDL_HitTestResult OnHitTest(SDL_Window* window, const SDL_Point* points, void* data);

	private:
		virtual void InitWindow(const WindowProps& props);
	private:
		SDL_Window* m_Window;
		SDL_Event m_Event;

		OpenGLContext* m_Context;

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;
			bool Maximized;
			bool Minimized = false;
			bool Fullscreen;
			RendererAPI::API API;

			EventCallbackFn EventCallback;
			std::function<HitTest(int, int)> HitTestCallback = nullptr;
		};

		WindowData m_Data;

		friend class WindowsImGuiPlatform;
	};

}
