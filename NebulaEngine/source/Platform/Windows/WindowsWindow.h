#pragma once

#include <SDL.h>

#include "Nebula/CoreSystems/Windows/Window.h"
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

		[[nodiscard]] virtual inline std::pair<unsigned int, unsigned int> GetSize() const override { return { m_Data.Width, m_Data.Height }; }
		virtual void SetSize(int w, int h) override;
		[[nodiscard]] virtual inline std::pair<unsigned int, unsigned int> GetPixelSize() const override;
		[[nodiscard]] virtual inline std::pair<unsigned int, unsigned int> GetPos() const override;
		virtual void SetPos(int x, int y) override;

		virtual void SetMousePos(int x, int y) const override;

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
		[[nodiscard]] virtual bool IsMinimized() const override { return m_Data.Minimized; }
		[[nodiscard]] virtual bool IsHighDPI() const override { return m_Data.HighDPI; }
		[[nodiscard]] virtual bool IsFocused() const override;
		virtual void Raise() const override;
		virtual std::string GetTitle() const override { return m_Data.Title; }
		virtual void SetTitle(const std::string& title) override;
		virtual void SetAlpha(float alpha) override;
		virtual void SetParentWindow(Ref<Window> parent) const override;

		virtual void Show() const override;
		virtual void Hide() const override;

		virtual void StartTextInput() const override;
		virtual void SetInputArea(int x, int y, int width, int height) const override;
		virtual void EndTextInput() const override;

		virtual void Flash() const override;

		[[nodiscard]] virtual Ref<GraphicsContext> GetContext() const override { return m_GraphicsContext; }
		virtual void SetCurrentContext() override;

		[[nodiscard]] inline virtual void* GetNativeWindow() const override { return m_Window; }
		[[nodiscard]] inline virtual void* GetHWND() const override { return SDL_GetPointerProperty(SDL_GetWindowProperties(m_Window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr); }
		[[nodiscard]] inline virtual uint32_t GetID() const override { return SDL_GetWindowID(m_Window); }

		static bool OnEvent(void* userdata, SDL_Event* e);
		static SDL_HitTestResult OnHitTest(SDL_Window* window, const SDL_Point* points, void* data);

	private:
		virtual void InitWindow(const WindowProps& props);
	private:
		SDL_Window* m_Window;
		SDL_Event m_Event;

		Ref<GraphicsContext> m_GraphicsContext;

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;
			bool Maximized;
			bool Minimized = false;
			bool Fullscreen;
			bool HighDPI;
			RendererAPI::API API;

			EventCallbackFn EventCallback;
			std::function<HitTest(int, int)> HitTestCallback = nullptr;
		};

		WindowData m_Data;

		friend class WindowsImGuiPlatform;
	};

}
