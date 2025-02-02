#pragma once

#include "../Events/Event.h"

namespace Nebula {
	struct WindowProps
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;

		WindowProps(const std::string& title = "Nebula Engine",
			unsigned int width = 1920,
			unsigned int height = 1080)
			: Title(title), Width(width), Height(height)
		{
		}
	};

	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void OnUpdate() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;
		virtual unsigned int GetTime() const = 0;

		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void SetCurrentContext() = 0;

		virtual void* GetNativeWindow() const = 0;
		virtual uint32_t GetID() const = 0;

		static Ref<Window> GetWindow(uint32_t windowID);
		static void DestroyWindow(Ref<Window> window);

		static Ref<Window> Create(const WindowProps& props = WindowProps());

	private:
		static std::vector<Ref<Window>> m_Windows;
	};

}