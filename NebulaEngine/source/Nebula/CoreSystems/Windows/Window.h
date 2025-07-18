#pragma once

#include "Nebula/Events/Event.h"
#include "Nebula/Renderer/Core/RendererAPI.h"

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
		enum class HitTest
		{
			None,
			Draggable,
			ResizeTopLeft,
			ResizeTop,
			ResizeTopRight,
			ResizeRight,
			ResizeBottomRight,
			ResizeBottom,
			ResizeBottomLeft,
			ResizeLeft
		};

		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() = default;

		virtual void OnUpdate() = 0;

		virtual void Shutdown() = 0;

		[[nodiscard]] virtual unsigned int GetWidth() const = 0;
		[[nodiscard]] virtual unsigned int GetHeight() const = 0;
		[[nodiscard]] virtual unsigned int GetPosX() const = 0;
		[[nodiscard]] virtual unsigned int GetPosY() const = 0;
		[[nodiscard]] virtual unsigned int GetTime() const = 0;

		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetWindowHitTest(std::function<HitTest(int, int)> callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		[[nodiscard]] virtual bool IsVSync() const = 0;
		virtual void SetFullscreen(bool enabled) = 0;
		[[nodiscard]] virtual bool IsFullscreen() const = 0;
		virtual void SetMaximized(bool enabled) = 0;
		[[nodiscard]] virtual bool IsMaximized() const = 0;
		virtual void Minimize() = 0;

		virtual void Flash() const = 0;

		virtual void SetCurrentContext() = 0;

		[[nodiscard]] virtual void* GetNativeWindow() const = 0;
		[[nodiscard]] virtual uint32_t GetID() const = 0;

		static Ref<Window> GetWindow(uint32_t windowID);
		static void DestroyWindow(uint32_t windowID);

		static Ref<Window> Create(const WindowProps& props = WindowProps());

	private:
		static std::vector<Ref<Window>> m_Windows;
	};

}
