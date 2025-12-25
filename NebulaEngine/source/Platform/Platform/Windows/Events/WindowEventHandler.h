#pragma once

#include <winrt/base.h>
#include <winrt/Microsoft.UI.Input.h>
#include <winrt/Microsoft.UI.Windowing.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Microsoft.UI.h>

#include "Platform/Nebula/CoreSystems/Events/Event.h"
#include "Platform/Nebula/CoreSystems/Windows/Window.h"

namespace Nebula::Platform
{

    class WindowEventHandler
    {
    public:
        WindowEventHandler(Window* window);
        ~WindowEventHandler();

        void InitEventHandlers();

        void SetEventCallback(const std::function<void(Event&)>& callback) { m_EventCallback = callback; }

    public:
        void WindowClosedHandler(winrt::Microsoft::UI::Windowing::AppWindow window, winrt::Microsoft::UI::Windowing::AppWindowClosingEventArgs args);
        void WindowChangedHandler(winrt::Microsoft::UI::Windowing::AppWindow window, winrt::Microsoft::UI::Windowing::AppWindowChangedEventArgs args);

        void WindowActivationChangedHandler(winrt::Microsoft::UI::Input::InputActivationListener listener, winrt::Microsoft::UI::Input::InputActivationListenerActivationChangedEventArgs args);

        void DisplayAreaAddedHandler(winrt::Microsoft::UI::Windowing::DisplayAreaWatcher watcher, winrt::Microsoft::UI::Windowing::DisplayArea display);
        void DisplayAreaRomovedHandler(winrt::Microsoft::UI::Windowing::DisplayAreaWatcher watcher, winrt::Microsoft::UI::Windowing::DisplayArea display);
        void DisplayAreaUpdatedHandler(winrt::Microsoft::UI::Windowing::DisplayAreaWatcher watcher, winrt::Microsoft::UI::Windowing::DisplayArea display);

    private:
        Window* m_Window;

        std::function<void(Event&)> m_EventCallback;

        winrt::Microsoft::UI::Input::InputActivationListener m_InputActivationListener;
        winrt::Microsoft::UI::Windowing::DisplayAreaWatcher m_DisplayAreaWatcher;
    };

}
