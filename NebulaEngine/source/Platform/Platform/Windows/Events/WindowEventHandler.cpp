#include "WindowEventHandler.h"

#include "Platform/Nebula/CoreSystems/Events/WindowEvents.h"

namespace Nebula::Platform
{
    WindowEventHandler::WindowEventHandler(Window* window)
        : m_Window{window}, m_InputActivationListener{nullptr}, m_DisplayAreaWatcher{nullptr}
    {
        winrt::Microsoft::UI::Windowing::AppWindow* NativeWindow = static_cast<winrt::Microsoft::UI::Windowing::AppWindow*>(window->GetNativeWindow());
        m_InputActivationListener = winrt::Microsoft::UI::Input::InputActivationListener::GetForWindowId(NativeWindow->Id());
        m_DisplayAreaWatcher = winrt::Microsoft::UI::Windowing::DisplayArea::CreateWatcher();
        m_DisplayAreaWatcher.Start();
    }

    WindowEventHandler::~WindowEventHandler()
    {
        m_DisplayAreaWatcher.Stop();
    }

    void WindowEventHandler::InitEventHandlers()
    {
        winrt::Microsoft::UI::Windowing::AppWindow* NativeWindow = static_cast<winrt::Microsoft::UI::Windowing::AppWindow*>(m_Window->GetNativeWindow());

        NativeWindow->Closing({ this, &WindowEventHandler::WindowClosedHandler });
        NativeWindow->Changed({ this, &WindowEventHandler::WindowChangedHandler });

        m_InputActivationListener.InputActivationChanged({ this, &WindowEventHandler::WindowActivationChangedHandler });

        m_DisplayAreaWatcher.Added({ this, &WindowEventHandler::DisplayAreaAddedHandler });
        m_DisplayAreaWatcher.Removed({ this, &WindowEventHandler::DisplayAreaRomovedHandler });
        m_DisplayAreaWatcher.Updated({ this, &WindowEventHandler::DisplayAreaUpdatedHandler });
    }

    void WindowEventHandler::WindowClosedHandler(winrt::Microsoft::UI::Windowing::AppWindow window,
                                                 winrt::Microsoft::UI::Windowing::AppWindowClosingEventArgs args)
    {
        args.Cancel(true);
        WindowClosedEvent event = WindowClosedEvent();
        event.WindowID = window.Id().Value;
        m_EventCallback(event);
    }

    void WindowEventHandler::WindowChangedHandler(winrt::Microsoft::UI::Windowing::AppWindow window,
        winrt::Microsoft::UI::Windowing::AppWindowChangedEventArgs args)
    {
        if (args.DidPositionChange())
        {
            m_Window->GetProperties().x = window.Position().X;
            m_Window->GetProperties().y = window.Position().Y;

            WindowMovedEvent event = WindowMovedEvent(window.Position().X, window.Position().Y);
            event.WindowID = window.Id().Value;
            m_EventCallback(event);
        }

        if (args.DidSizeChange())
        {
            m_Window->GetProperties().Width = window.Size().Width;
            m_Window->GetProperties().Height = window.Size().Height;

            WindowResizedEvent event = WindowResizedEvent(window.Size().Width, window.Size().Height);
            event.WindowID = window.Id().Value;
            m_EventCallback(event);
        }

        if (args.DidPresenterChange())
        {
            if (window.Presenter().Kind() == winrt::Microsoft::UI::Windowing::AppWindowPresenterKind::Overlapped)
            {
                winrt::Microsoft::UI::Windowing::OverlappedPresenter presenter = window.Presenter().as<winrt::Microsoft::UI::Windowing::OverlappedPresenter>();

                if (presenter.State() == winrt::Microsoft::UI::Windowing::OverlappedPresenterState::Maximized)
                {
                    m_Window->GetProperties().Maximized = true;
                    m_Window->GetProperties().Minimized = false;

                    WindowMaximizedEvent maxEvent = WindowMaximizedEvent();
                    maxEvent.WindowID = window.Id().Value;
                    m_EventCallback(maxEvent);
                }

                if (presenter.State() == winrt::Microsoft::UI::Windowing::OverlappedPresenterState::Restored)
                {
                    m_Window->GetProperties().Maximized = false;
                    m_Window->GetProperties().Minimized = false;

                    WindowRestoredEvent restoreEvent = WindowRestoredEvent();
                    restoreEvent.WindowID = window.Id().Value;
                    m_EventCallback(restoreEvent);
                }

                if (presenter.State() == winrt::Microsoft::UI::Windowing::OverlappedPresenterState::Minimized)
                {
                    m_Window->GetProperties().Maximized = false;
                    m_Window->GetProperties().Minimized = true;

                    WindowMinimizedEvent minEvent = WindowMinimizedEvent();
                    minEvent.WindowID = window.Id().Value;
                    m_EventCallback(minEvent);
                }
            }
        }
    }

    void WindowEventHandler::WindowActivationChangedHandler(
        winrt::Microsoft::UI::Input::InputActivationListener listener,
        winrt::Microsoft::UI::Input::InputActivationListenerActivationChangedEventArgs args)
    {
        winrt::Microsoft::UI::Windowing::AppWindow* NativeWindow = static_cast<winrt::Microsoft::UI::Windowing::AppWindow*>(m_Window->GetNativeWindow());

        if (listener.State() == winrt::Microsoft::UI::Input::InputActivationState::Activated)
        {
            WindowFocusedEvent event = WindowFocusedEvent();
            event.WindowID = NativeWindow->Id().Value;
            m_EventCallback(event);
        }

        if (listener.State() == winrt::Microsoft::UI::Input::InputActivationState::Deactivated)
        {
            WindowLostFocusEvent event = WindowLostFocusEvent();
            event.WindowID = NativeWindow->Id().Value;
            m_EventCallback(event);
        }
    }

    void WindowEventHandler::DisplayAreaAddedHandler(winrt::Microsoft::UI::Windowing::DisplayAreaWatcher watcher,
        winrt::Microsoft::UI::Windowing::DisplayArea display)
    {
        winrt::Microsoft::UI::Windowing::AppWindow* NativeWindow = static_cast<winrt::Microsoft::UI::Windowing::AppWindow*>(m_Window->GetNativeWindow());

        MonitorsChangedEvent event = MonitorsChangedEvent();
        event.WindowID = NativeWindow->Id().Value;
        m_EventCallback(event);
    }

    void WindowEventHandler::DisplayAreaRomovedHandler(winrt::Microsoft::UI::Windowing::DisplayAreaWatcher watcher,
        winrt::Microsoft::UI::Windowing::DisplayArea display)
    {
        winrt::Microsoft::UI::Windowing::AppWindow* NativeWindow = static_cast<winrt::Microsoft::UI::Windowing::AppWindow*>(m_Window->GetNativeWindow());

        MonitorsChangedEvent event = MonitorsChangedEvent();
        event.WindowID = NativeWindow->Id().Value;
        m_EventCallback(event);
    }

    void WindowEventHandler::DisplayAreaUpdatedHandler(winrt::Microsoft::UI::Windowing::DisplayAreaWatcher watcher,
        winrt::Microsoft::UI::Windowing::DisplayArea display)
    {
        winrt::Microsoft::UI::Windowing::AppWindow* NativeWindow = static_cast<winrt::Microsoft::UI::Windowing::AppWindow*>(m_Window->GetNativeWindow());

        MonitorsChangedEvent event = MonitorsChangedEvent();
        event.WindowID = NativeWindow->Id().Value;
        m_EventCallback(event);
    }
}
