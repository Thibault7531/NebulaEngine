#pragma once

#include "Nebula/Events/Event.h"
#include "Nebula/Renderer/Core/GraphicsContext.h"
#include "Nebula/Renderer/Core/RendererAPI.h"

namespace Nebula {

	struct WindowProps
	{
		std::string Title = "NebulaEngine";

		unsigned int Width = 1920;
		unsigned int Height = 1080;

		int x = -INT_MAX;
		int y = -INT_MAX;

		bool Border = true;
		bool Maximized = false;
		bool Fullscreen = false;

		bool isResizable = true;
		bool AlwaysOnTop = false;
		bool HighDPI = false;
		bool isUtility = false;

		bool isImGui = false;

		RendererAPI::API API = RendererAPI::GetAPI();
	};

	class Window
	{
	public:
		enum class HitTest
		{
			None,
			Client,
			TitleBar,
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

		virtual void SwapBuffers() const = 0;

		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetWindowHitTest(std::function<HitTest(int, int)> callback) = 0;

		[[nodiscard]] virtual std::pair<unsigned int, unsigned int> GetSize() const = 0;
		virtual void SetSize(int w, int h) = 0;
		[[nodiscard]] virtual std::pair<unsigned int, unsigned int> GetPixelSize() const = 0;
		[[nodiscard]] virtual std::pair<int, int> GetPos() const = 0;
		virtual void SetPos(int x, int y) = 0;

		virtual void SetMousePos(int x, int y) const = 0;

		virtual void SetVSync(bool enabled) = 0;
		[[nodiscard]] virtual bool IsVSync() const = 0;
		virtual void SetFullscreen(bool enabled) = 0;
		[[nodiscard]] virtual bool IsFullscreen() const = 0;
		virtual void SetMaximized(bool enabled) = 0;
		[[nodiscard]] virtual bool IsMaximized() const = 0;
		virtual void Minimize() = 0;
		[[nodiscard]] virtual bool IsMinimized() const = 0;
		[[nodiscard]] virtual bool IsHighDPI() const = 0;
		[[nodiscard]] virtual bool IsFocused() const = 0;
		[[nodiscard]] virtual std::string GetTitle() const = 0;
		virtual void SetTitle(const std::string& title) = 0;
		virtual void SetAlpha(float alpha) = 0;
		virtual void SetParentWindow(Ref<Window> parent) const = 0;

		virtual void Show() const = 0;
		virtual void Hide() const = 0;
		virtual void Raise() const = 0;
		virtual void Flash() const = 0;

		[[nodiscard]] virtual Ref<GraphicsContext> GetContext() const = 0;
		virtual void SetCurrentContext() = 0;

		[[nodiscard]] virtual void* GetNativeWindow() const = 0;
		[[nodiscard]] virtual void* GetHandle() const = 0;
		[[nodiscard]] virtual uint32_t GetID() const = 0;
		virtual void SetID(int ID) = 0;

	public:
		static Ref<Window> Create(const WindowProps& props = WindowProps());
	};

}
