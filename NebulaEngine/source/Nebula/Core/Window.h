#pragma once

#include "../Events/Event.h"
#include "Nebula/Renderer/RendererAPI.h"

namespace Nebula {

	struct WindowProps
	{
		std::string Title = "NebulaTech";

		unsigned int Width = 1920;
		unsigned int Height = 1080;

		bool Border = true;
		bool Maximized = false;
		bool Fullscreen = false;

		bool isResizable = true;
		bool AlwaysOnTop = false;
		bool HighDPI = false;
		bool isUtility = false;

		RendererAPI::API API = RendererAPI::API::OpenGL;
	};

	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void OnUpdate() = 0;

		virtual void Shutdown() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;
		virtual unsigned int GetTime() const = 0;

		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;
		virtual void SetFullscreen(bool enabled) = 0;
		virtual bool IsFullscreen() const = 0;
		virtual void SetMaximized(bool enabled) = 0;
		virtual bool IsMaximized() const = 0;

		virtual void Flash() const = 0;

		virtual void SetCurrentContext() = 0;

		virtual void* GetNativeWindow() const = 0;
		virtual uint32_t GetID() const = 0;

		static Ref<Window> GetWindow(uint32_t windowID);
		static void DestroyWindow(uint32_t windowID);

		static Ref<Window> Create(const WindowProps& props = WindowProps());

	private:
		static std::vector<Ref<Window>> m_Windows;
	};

}
