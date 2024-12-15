#pragma once

#include "Nebula/Events/Event.h"

namespace Nebula {
	/**
	 * Information about the window.
	 */
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

	/**
	 * Window interface.
	 */
	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {}

		/**
		 * Function to be called when an update arrives.
		 */
		virtual void OnUpdate() = 0;

		/**
		 * Gets the width of the window.
		 * @return Width of the window
		 */
		virtual unsigned int GetWidth() const = 0;
		/**
		 * Gets the height of the window.
		 * @return Height of the window
		 */
		virtual unsigned int GetHeight() const = 0;

		/**
		 * Sets the function to be called when an event arrives.
		 * @param callback Function to call to dispatch incoming events
		 */
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		/**
		 * Toggles Vsync.
		 * @param enabled Whether Vsync should be on
		 */
		virtual void SetVSync(bool enabled) = 0;
		/**
		 * Gets if VSync is on
		 * @return Is Vsync on
		 */
		virtual bool IsVSync() const = 0;

		/**
		 * Gets the native window handle.
		 * @return Native window handle
		 */
		virtual void* GetNativeWindow() const = 0;

		/**
		 * Creates a window.
		 * @param props Window information
		 * @return Pointer to the window
		 */
		static Window* Create(const WindowProps& props = WindowProps());
	};

}